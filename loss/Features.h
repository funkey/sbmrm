#ifndef SBMRM_LOSS_FEATURES_H__
#define SBMRM_LOSS_FEATURES_H__

#include <util/assert.h>
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
								boost::lexical_cast<std::string>(f.size()) +
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

		UTIL_ASSERT_REL(w.size(), ==, _numFeatures);
		UTIL_ASSERT_REL(f.size(), ==, _features.size());

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

		UTIL_ASSERT_REL(y.size(), ==, _features.size());
		UTIL_ASSERT_REL(e.size(), ==, _numFeatures);

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

	/**
	 * Normalize all features, such that their absolute values are in the range 
	 * [0,1].
	 */
	void normalize() {

		_min = std::vector<double>(_numFeatures, std::numeric_limits<double>::max());
		_max = std::vector<double>(_numFeatures, std::numeric_limits<double>::min());

		// find min and max
		foreach (const std::vector<double>& f, _features) {

			for (unsigned int i = 0; i < _numFeatures; i++) {

				_min[i] = std::min(_min[i], f[i]);
				_max[i] = std::max(_max[i], f[i]);
			}
		}

		// scale features
		foreach (std::vector<double>& f, _features)
			normalize(f);
	}

	/**
	 * Normalize a single feature vector in the same way, the features of this 
	 * set have been noramlized already (this assumes that noramlize() was 
	 * called already). Use this to convert the learnt weight vector into the 
	 * original feature space.
	 */
	inline void normalize(std::vector<double>& f) {

		for (unsigned int i = 0; i < _numFeatures; i++) {

			double maxAbs = std::max(std::abs(_min[i]), std::abs(_max[i]));

			if (maxAbs > 0)
				f[i] = f[i]/maxAbs;
		}
	}

private:

	unsigned int _numFeatures;

	std::vector<std::vector<double> > _features;

	std::vector<double> _min;
	std::vector<double> _max;
};

#endif // SBMRM_LOSS_FEATURES_H__

