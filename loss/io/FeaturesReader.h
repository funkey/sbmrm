#ifndef SBMRM_LOSS_IO_FEATURES_READER_H__
#define SBMRM_LOSS_IO_FEATURES_READER_H__

#include <pipeline/SimpleProcessNode.h>
#include <loss/Features.h>

class FeaturesReader : public pipeline::SimpleProcessNode<> {

public:

	/**
	 * Create a feature reader.
	 *
	 * @param filename
	 *              The file to read the features from.
	 * @param normalize
	 *              Normalize the features after reading, such that their 
	 *              absolute values are in the range [0,1]. If you do that, make 
	 *              sure to denormalize the learnt weights accordingly.
	 */
	FeaturesReader(std::string filename, bool normalize = false);

private:

	void updateOutputs();

	pipeline::Output<Features> _features;

	std::string _filename;

	bool _normalize;
};

#endif // SBMRM_LOSS_IO_FEATURES_READER_H__

