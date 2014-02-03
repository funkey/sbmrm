#ifndef SBMRM_INFERENCE_IO_CONSTRAINTS_READER_H__
#define SBMRM_INFERENCE_IO_CONSTRAINTS_READER_H__

#include <string>

#include <pipeline/SimpleProcessNode.h>
#include <inference/LinearConstraints.h>

class ConstraintsReader : public pipeline::SimpleProcessNode<> {

public:

	ConstraintsReader(std::string filename);

private:

	void updateOutputs();

	void readConstraint(std::string& line);

	pipeline::Output<LinearConstraints> _constraints;

	std::string _filename;
};

#endif // SBMRM_INFERENCE_IO_CONSTRAINTS_READER_H__

