#include "BundleCollector.h"

BundleCollector::BundleCollector() {

	registerOutput(_constraints, "linear constraints");

	_constraints.registerForwardSlot(_constraintAdded);
}

void
BundleCollector::addHyperplane(std::vector<double>& a, double b) {
	/*
	  <w,a> + b ≤  ξ
	        <=>
	  <w,a> - ξ ≤ -b
	*/

	unsigned int dims = a.size();

	LinearConstraint constraint;

	for (unsigned int i = 0; i < dims; i++)
		constraint.setCoefficient(i, a[i]);
	constraint.setCoefficient(dims, -1.0);
	constraint.setRelation(LessEqual);
	constraint.setValue(-b);

	_constraints->add(constraint);

	_constraintAdded();
}
