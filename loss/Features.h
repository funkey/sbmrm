#ifndef SBMRM_LOSS_FEATURES_H__
#define SBMRM_LOSS_FEATURES_H__

#include <util/exceptions.h>

/**
 * The feature matrix φ(x'). Each column is a feature vector for one component 
 * of y, such that the energy for each y is: E(y) = <w,φ(x')y>.
 */
class Features {

public:

	/**
	 * Add a new feature vector.
	 */
	void addFeatureVector(std::vector<double>& f) {

		if (_features.size() == 0)
			_numFeatures = f.size();
		else
			if (f.size() != _numFeatures)
				BOOST_THROW_EXCEPTION(
						SizeMismatchError() <<
								error_message(std::string("number of features added (") +
								boost::lexical_cast<std::string>(_features.size()) +
								") does not match expected number (" +
								boost::lexical_cast<std::string>(_numFeatures) + ")"));

		_features.push_back(f);
	}

	/**
	 * Get the feature vector for the ith component of y.
	 */
	const std::vector<double>& getFeatureVector(unsigned int i) const {

		return _features[i];
	}

	/**
	 * For a given set of feature weights w, get the coefficients f := wφ(x'), 
	 * such that E(y) = <f,y>.
	 */
	void getCoefficients(const std::vector<double>& w, std::vector<double>& f) const {

		std::fill(f.begin(), f.end(), 0.0);

		for (unsigned int i = 0; i < _features.size(); i++)
			for (unsigned int j = 0; j < w.size(); j++)
				f[i] += w[j]*_features[i][j];
	}

	/**
	 * For a given assignment of y, get the combined feature vector e := φ(x')y, 
	 * such that E(y) = <w,e>.
	 */
	void combineFeatures(const std::vector<double>& y, std::vector<double>& e) const {

		std::fill(e.begin(), e.end(), 0.0);

		for (unsigned int i = 0; i < _features.size(); i++)
			for (unsigned int j = 0; j < e.size(); j++)
				e[j] += y[i]*_features[i][j];
	}

	/**
	 * The number of features per feature vector.
	 */
	unsigned int numFeatures() const {

		return _numFeatures;
	}

	/**
	 * The number of feature vectors.
	 */
	unsigned int numFeatureVectors() const {

		return _features.size();
	}

	/**
	 * Remove all features.
	 */
	void clear() {

		_features.clear();
	}

private:

	unsigned int _numFeatures;

	std::vector<std::vector<double> > _features;
};

#endif // SBMRM_LOSS_FEATURES_H__

