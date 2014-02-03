#include <fstream>

#include <util/files.h>
#include <util/Logger.h>
#include "ConstraintsReader.h"

logger::LogChannel constraintsreaderlog("constraintsreaderlog", "[ConstraintsReader] ");

ConstraintsReader::ConstraintsReader(std::string filename) :
		_filename(filename) {

	registerOutput(_constraints, "linear constraints");
}

void
ConstraintsReader::updateOutputs() {

	std::ifstream in(_filename.c_str());

	while (!in.eof() && in.good()) {

		std::string line = readline(in);

		if (line.size() > 0)
			readConstraint(line);
	}
}

void
ConstraintsReader::readConstraint(std::string& line) {

	LinearConstraint constraint;

	std::string whitespace = " \t";
	std::string number = "0123456789.eE-+";

	if (line[line.find_first_not_of(whitespace)] == '#')
		return;

	// position of relation
	size_t r = line.find_first_of("<>=");

	if (r == std::string::npos) {

		LOG_ERROR(constraintsreaderlog) << "found corrupted line" << std::endl;
		return;
	}

	// to first coefficient
	size_t i = line.find_first_not_of(whitespace);
	size_t j;

	// read coefficients
	while (i < r) {

		// first after coefficient
		j = line.find_first_not_of(number, i);

		double coef = boost::lexical_cast<double>(line.substr(i, j - i));

		// to variable number
		i = line.find_first_of(number, j);
		// first after variable number
		j = line.find_first_not_of(number, i);

		unsigned int var = boost::lexical_cast<unsigned int>(line.substr(i, j - i));

		// to next coefficient
		i = line.find_first_of(number, j);

		constraint.setCoefficient(var, coef);
	}

	// read relation
	i = r;
	if (line.substr(i, 2) == "<=")
		constraint.setRelation(LessEqual);
	else if (line.substr(i, 2) == "==")
		constraint.setRelation(Equal);
	else if (line.substr(i, 2) == ">=")
		constraint.setRelation(GreaterEqual);
	else
		LOG_ERROR(constraintsreaderlog) << "unknown relation " << line.substr(i, 2) << std::endl;

	i = line.find_first_of(number, r);
	j = line.find_first_not_of(number, i);

	// read value
	//LOG_ALL(constraintsreaderlog) << "i = " << i << ", j = " << j << ", substr = " << line.substr(i, j - i) << std::endl;
	double value = boost::lexical_cast<double>(line.substr(i, j - i));

	constraint.setValue(value);

	LOG_ALL(constraintsreaderlog) << "read constraint " << constraint << std::endl;

	// add the constraint
	_constraints->add(constraint);
}
