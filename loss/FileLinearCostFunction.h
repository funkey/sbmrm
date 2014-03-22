#ifndef SBMRM_FILE_LINEAR_COST_FUNCTION_H__
#define SBMRM_FILE_LINEAR_COST_FUNCTION_H__

#include <string>
#include "LinearCostFunction.h"

class FileLinearCostFunction : public LinearCostFunction {

public:

	FileLinearCostFunction(std::string filename);

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

#endif // SBMRM_FILE_LINEAR_COST_FUNCTION_H__

