#ifndef SBMRM_LOSS_LINEAR_COST_FUNCTION_H__
#define SBMRM_LOSS_LINEAR_COST_FUNCTION_H__

#include <vector>

/**
 * Interface for linear cost functions
 *
 *   Δ(y',y) = <a(y'),y> + b.
 */
class LinearCostFunction {

public:

	/**
	 * Get the linear coefficients a(y').
	 */
	virtual std::vector<double> getCoefficients() = 0;

	/**
	 * Get the constant offset b.
	 */
	virtual double getConstantOffset() = 0;
};

#endif // SBMRM_LOSS_LINEAR_COST_FUNCTION_H__

