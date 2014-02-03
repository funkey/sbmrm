#include <util/Logger.h>
#include <util/helpers.hpp>
#include "SoftMarginLoss.h"

logger::LogChannel softmarginlosslog("softmarginlosslog", "[SoftMarginLoss] ");

SoftMarginLoss::SoftMarginLoss(
		LinearCostFunction&                   costs,
		pipeline::Value<LinearConstraints>    constraints,
		pipeline::Value<Features>             features,
		pipeline::Value<std::vector<double> > groundTruth) :

		_features(features),
		_groundTruth(groundTruth) {

	_f.resize(_groundTruth->size(), 0.0);

	// cost function Δ(y',y) = <g,y> + b
	_b = costs.getConstantOffset();
	_g = costs.getCoefficients();

	LOG_ALL(softmarginlosslog) << "cost function linear   contribution is : " << _g << std::endl;
	LOG_ALL(softmarginlosslog) << "cost function constant contribution is : " << _b << std::endl;

	// combined features of ground truth
	_d.resize(_features->numFeatures());
	_features->combineFeatures(*_groundTruth, _d);

	LOG_ALL(softmarginlosslog) << "φ(x')y' = " << _d << std::endl;

	// combined features of current y*
	_e.resize(_features->numFeatures());

	// setup objective
	_objective->resize(_groundTruth->size());
	_objective->setSense(Maximize);

	// setup ILP pipeline
	_parameters->setVariableType(Binary);
	_solver->setInput("objective", _objective);
	_solver->setInput("linear constraints", constraints);
	_solver->setInput("parameters", _parameters);
	_solution = _solver->getOutput("solution");
}

void
SoftMarginLoss::valueAndGradient(const std::vector<double>& w, double& value, std::vector<double>& gradient) {

	// L(w) = max_y <w,φ(x')y' - φ(x')y>     + Δ(y',y)
	//      = max_y <wφ(x'),y'-y>            + Δ(y',y)
	//      = max_y <wφ(x'),y'> - <wφ(x'),y> + Δ(y',y)
	//
	//   f := wφ(x')

	_features->getCoefficients(w, _f);

	LOG_ALL(softmarginlosslog) << "wφ(x') = " << _f << std::endl;

	//
	//      = max_y <f,y'>  - <f,y> +  Δ(y',y)
	//      = max_y    a    - <f,y> +  b + <g, y>

	double a = dot(_f, *_groundTruth);

	//      = max_y (a + b) + <(g - f),y>

	// update objective
	_objective->setConstant(a + _b);
	for (unsigned int i = 0; i < _f.size(); i++)
		_objective->setCoefficient(i, _g[i] - _f[i]);

	LOG_ALL(softmarginlosslog) << "objective is " << *_objective << std::endl;

	// let solver know we changed the objective
	_solver->setInput("objective", _objective);

	// read optimal value L(w)
	value = _solution->getValue();

	// ∂L(w)/∂w = φ(x')y' - φ(x')y*
	//          = d       - e

	// compute gradient
	gradient = _d;
	_features->combineFeatures(_solution->getVector(), _e);
	for (unsigned int i = 0; i < gradient.size(); i++)
		gradient[i] -= _e[i];
}

double
SoftMarginLoss::dot(std::vector<double>& a, std::vector<double>& b) {

	assert(a.size() == b.size());

	double d = 0.0;
	for (unsigned int i = 0; i < a.size(); i++)
		d += a[i]*b[i];

	return d;
}
