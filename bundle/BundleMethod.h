#ifndef SBMRM_BUNDLE_METHOD_H__
#define SBMRM_BUNDLE_METHOD_H__

#include <vector>
#include <boost/function.hpp>

#include <pipeline/Value.h>
#include <pipeline/Process.h>
#include <inference/QuadraticSolver.h>

#include "BundleCollector.h"

/**
 * Implements a bundle method with a quadratic regularizer for arbitrary convex 
 * functions.
 */
class BundleMethod {

public:

	typedef boost::function<void(const std::vector<double>& w, double& value, std::vector<double>& gradient)> callback_t;

	/**
	 * Create a new bundle method for the given value and gradient callback.
	 *
	 * @param valueGradientCallback
	 *           A function returning the value and gradient at a given position 
	 *           w.
	 * @param dims
	 *           The size of the vector w.
	 * @param regularizerWeight
	 *           The weight of the quadratic regularizer.
	 * @param eps
	 *           Convergence threshold.
	 */
	BundleMethod(
			callback_t valueGradientCallback,
			unsigned int dims,
			double regularizerWeight,
			double eps);

	/**
	 * Start the optimization.
	 *
	 * @return The w minimizing the regularized version of the given function.
	 */
	std::vector<double> optimize();

private:

	void setupQp();

	void findMinLowerBound(std::vector<double>& w, double& value);

	inline double dot(std::vector<double>& a, std::vector<double>& b);

	// callback providing L(w) and ∂L(w)/∂w
	callback_t _valueGradientCallback;

	// the size of w
	unsigned int _dims;

	// the weight of the regularizer
	double _lambda;

	// convergence threshold
	double _eps;

	pipeline::Value<QuadraticObjective>        _qpObjective;
	pipeline::Value<QuadraticSolverParameters> _qpParameters;

	pipeline::Process<BundleCollector> _bundleCollector;
	pipeline::Process<QuadraticSolver> _qpSolver;

	pipeline::Value<Solution> _qpSolution;
};

#endif // SBMRM_BUNDLE_METHOD_H__

