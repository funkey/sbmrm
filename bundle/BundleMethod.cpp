#include <limits>

#include <util/helpers.hpp>
#include <util/Logger.h>
#include <util/ProgramOptions.h>
#include "BundleMethod.h"

logger::LogChannel bundlelog("bundlelog", "[BundleMethod] ");

util::ProgramOption optionOutputPrecision(
		util::_long_name        = "outputPrecision",
		util::_description_text = "The decimal precision of printed numbers in the bundle method.");

BundleMethod::BundleMethod(callback_t valueGradientCallback, unsigned int dims, double regularizerWeight, double eps) :
	_valueGradientCallback(valueGradientCallback),
	_dims(dims),
	_lambda(regularizerWeight),
	_eps(eps) {

	setupQp();

	if (optionOutputPrecision) {

		unsigned int precision = optionOutputPrecision;

		LOG_DEBUG(bundlelog) << std::setprecision(precision);
		LOG_ALL(bundlelog)   << std::setprecision(precision);
		LOG_ERROR(bundlelog) << std::setprecision(precision);
	}
}

std::vector<double>
BundleMethod::optimize() {

	/*
	  1. w_0 = 0, t = 0
	  2. t++
	  3. compute a_t = ∂L(w_t-1)/∂w
	  4. compute b_t =  L(w_t-1) - <w_t-1,a_t>
	  5. ℒ_t(w) = max_i <w,a_i> + b_i
	  6. w_t = argmin λ½|w|² + ℒ_t(w)
	  7. ε_t = min_i [ λ½|w_i|² + L(w_i) ] - [ λ½|w_t|² + ℒ_t(w_t) ]
			   ^^^^^^^^^^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^
				 smallest L(w) ever seen    current min of lower bound
	  8. if ε_t > ε, goto 2
	  9. return w_t
	*/

	std::vector<double> w(_dims, 0.0);
	double minValue = std::numeric_limits<double>::infinity();

	unsigned int t = 0;

	while (true) {

		t++;

		LOG_USER(bundlelog) << std::endl << "----------------- iteration " << t << std::endl;

		std::vector<double> w_tm1 = w;

		LOG_DEBUG(bundlelog) << "current w is " << w_tm1 << std::endl;

		// value of L at current w
		double L_w_tm1 = 0.0;

		// gradient of L at current w
		std::vector<double> a_t(_dims, 0.0);

		// get current value and gradient
		_valueGradientCallback(w_tm1, L_w_tm1, a_t);

		LOG_DEBUG(bundlelog) << "       L(w)              is: " << L_w_tm1 << std::endl;
		LOG_ALL(bundlelog)   << "      ∂L(w)/∂            is: " << a_t << std::endl;

		// update smallest observed value of regularized L
		minValue = std::min(minValue, L_w_tm1 + _lambda*0.5*dot(w_tm1, w_tm1));

		LOG_DEBUG(bundlelog) << " min_i L(w_i) + ½λ|w_i|² is: " << minValue << std::endl;

		// compute hyperplane offset
		double b_t = L_w_tm1 - dot(w_tm1, a_t);

		LOG_ALL(bundlelog) << "adding hyperplane " << a_t << "*w + " << b_t << std::endl;

		// update lower bound
		_bundleCollector->addHyperplane(a_t, b_t);

		// minimal value of lower bound
		double minLower;

		// update w and get minimal value
		findMinLowerBound(w, minLower);

		LOG_DEBUG(bundlelog) << " min_w ℒ(w)   + ½λ|w|²   is: " << minLower << std::endl;
		LOG_DEBUG(bundlelog) << " w* of ℒ(w)   + ½λ|w|²   is: "  << w << std::endl;

		// compute gap
		double eps_t = minValue - minLower;

		LOG_USER(bundlelog)  << "          ε   is: " << eps_t << std::endl;

		// converged?
		if (eps_t <= _eps) {

			if (eps_t >= 0) {

				LOG_USER(bundlelog) << "converged!" << std::endl;

			} else {

				LOG_ERROR(bundlelog) << "ε < 0 -- something went wrong" << std::endl;
				LOG_ERROR(bundlelog) << "(if |ε| is very small this might still be fine)" << std::endl;
			}

			break;
		}
	}

	return w;
}

void
BundleMethod::setupQp() {

	/*
	  w* = argmin λ½|w|² + ξ, s.t. <w,a_i> + b_i ≤ ξ ∀i
	*/

	// one variable for each component of w and for ξ
	_qpObjective->resize(_dims + 1);

	// regularizer
	for (unsigned int i = 0; i < _dims; i++)
		_qpObjective->setQuadraticCoefficient(i, i, 0.5*_lambda);

	// ξ
	_qpObjective->setCoefficient(_dims, 1.0);

	// we minimize
	_qpObjective->setSense(Minimize);

	// connect pipeline
	_qpSolver->setInput("objective", _qpObjective);
	_qpSolver->setInput("linear constraints", _bundleCollector->getOutput());
	_qpSolver->setInput("parameters", _qpParameters);
	_qpSolution = _qpSolver->getOutput("solution");
}

void
BundleMethod::findMinLowerBound(std::vector<double>& w, double& value) {

	// read the solution (pipeline magic!)
	for (unsigned int i = 0; i < _dims; i++)
		w[i] = (*_qpSolution)[i];

	value = _qpSolution->getValue();
}

double
BundleMethod::dot(std::vector<double>& a, std::vector<double>& b) {

	assert(a.size() == b.size());

	double d = 0.0;
	for (unsigned int i = 0; i < a.size(); i++)
		d += a[i]*b[i];

	return d;
}
