#ifndef SBMRM_LOSS_SOFT_MARGIN_H__
#define SBMRM_LOSS_SOFT_MARGIN_H__

#include <pipeline/Value.h>
#include <pipeline/Process.h>

#include <inference/LinearConstraints.h>
#include <inference/LinearObjective.h>
#include <inference/LinearSolver.h>
#include <inference/LinearSolverParameters.h>
#include <inference/Solution.h>
#include "Features.h"
#include "LinearCostFunction.h"

/**
 * Implements the soft margin loss, i.e.,
 *
 *   L(w) = max_y <w,φ(x')y' - φ(x')y> + Δ(y',y)
 *
 * for a ground truth y', features φ(x'), and a linear cost function Δ(y', y).  
 * The set of valid ys is given by linear constraints.
 */
class SoftMarginLoss {

public:

	/**
	 * Create a new soft margin loss function.
	 *
	 * @param costs
	 *             The linear cost function Δ(y',y) to use.
	 *
	 * @param constraints
	 *             Constraints on y.
	 *
	 * @param features
	 *             Features φ(x') for every component of y.
	 *
	 * @param groundTruth
	 *             The ground truth y'.
	 */
	SoftMarginLoss(
			LinearCostFunction&                   costs,
			pipeline::Value<LinearConstraints>    constraints,
			pipeline::Value<Features>             features,
			pipeline::Value<std::vector<double> > groundTruth);

	/**
	 * Computes the value and gradient of L(w).
	 */
	void valueAndGradient(const std::vector<double>& w, double& value, std::vector<double>& gradient);

private:

	inline double dot(std::vector<double>& a, std::vector<double>& b);

	pipeline::Value<Features>               _features;
	pipeline::Value<std::vector<double> >   _groundTruth;
	pipeline::Value<LinearSolverParameters> _parameters;
	pipeline::Value<LinearObjective>        _objective;
	pipeline::Process<LinearSolver>         _solver;
	pipeline::Value<Solution>               _solution;

	// the energy coefficients for y, f := wφ(x')
	std::vector<double> _f;

	// the linear and constant term of the cost function Δ(y',y)
	std::vector<double> _g;
	double              _b;

	// combined features of the ground truth and current y*
	std::vector<double> _d;
	std::vector<double> _e;
};

#endif // SBMRM_LOSS_SOFT_MARGIN_H__

