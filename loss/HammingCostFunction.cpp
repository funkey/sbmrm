#include <algorithm>
#include "HammingCostFunction.h"

HammingCostFunction::HammingCostFunction(const std::vector<double>& groundTruth) {

	// H(y',y) = Σ|y'_i - y_i|
	//         = Σ_{i:y'_i = 1} (1-y_i) + Σ_{i:y'_i = 0} y_i
	//         = |y'|² + Σ_{i:y'_i = 1} -y_i + Σ_{i:y'_i = 0} y_i
	//         =  c    + <l,y>
	//
	//   with l_i := -1 if y'_i = 1
	//                1 else

	_c = std::count(groundTruth.begin(), groundTruth.end(), 1.0);

	_l.resize(groundTruth.size());
	for (unsigned int i = 0; i < _l.size(); i++)
		_l[i] = (groundTruth[i] == 1.0 ? -1 : 1);
}
