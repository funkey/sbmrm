#ifndef SBMRM_LOSS_IO_GROUND_TRUTH_READER_H__
#define SBMRM_LOSS_IO_GROUND_TRUTH_READER_H__

#include <pipeline/SimpleProcessNode.h>

class GroundTruthReader : public pipeline::SimpleProcessNode<> {

public:

	GroundTruthReader(std::string filename);

private:

	void updateOutputs();

	pipeline::Output<std::vector<double> > _groundTruth;

	std::string _filename;
};

#endif // SBMRM_LOSS_IO_GROUND_TRUTH_READER_H__

