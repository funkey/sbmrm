#ifndef SBMRM_LOSS_IO_FEATURES_READER_H__
#define SBMRM_LOSS_IO_FEATURES_READER_H__

#include <pipeline/SimpleProcessNode.h>
#include <loss/Features.h>

class FeaturesReader : public pipeline::SimpleProcessNode<> {

public:

	FeaturesReader(std::string filename);

private:

	void updateOutputs();

	pipeline::Output<Features> _features;

	std::string _filename;
};

#endif // SBMRM_LOSS_IO_FEATURES_READER_H__

