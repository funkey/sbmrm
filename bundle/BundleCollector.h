#ifndef SBMRM_BUNDLE_COLLECTOR_H__
#define SBMRM_BUNDLE_COLLECTOR_H__

#include <pipeline/ProcessNode.h>
#include <pipeline/Output.h>

#include <inference/LinearConstraints.h>
#include <inference/Signals.h>

class BundleCollector : public pipeline::ProcessNode {

public:

	BundleCollector();

	void addHyperplane(std::vector<double>& a, double b);

private:

	pipeline::Output<LinearConstraints> _constraints;

	signals::Slot<ConstraintAdded> _constraintAdded;
};

#endif // SBMRM_BUNDLE_COLLECTOR_H__

