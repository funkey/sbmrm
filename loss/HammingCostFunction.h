#ifndef SBMRM_LOSS_HAMMING_COST_FUNCTION_H__
#define SBMRM_LOSS_HAMMING_COST_FUNCTION_H__

#include "LinearCostFunction.h"

class HammingCostFunction : public LinearCostFunction {

public:

	/**
	 * Create a Hamming cost function for the given ground truth vector.
	 */
	HammingCostFunction(const std::vector<double>& groundTruth);

	/**
	 * Get the linear coefficients a(y').
	 */
	std::vector<double> getCoefficients() { return _l; }

	/**
	 * Get the constant offset b.
	 */
	double getConstantOffset() { return _c; }

private:

	std::vector<double> _l;
	double              _c;
};

#endif // SBMRM_LOSS_HAMMING_COST_FUNCTION_H__

