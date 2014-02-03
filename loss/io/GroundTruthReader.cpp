#include <fstream>

#include <util/files.h>
#include "GroundTruthReader.h"

GroundTruthReader::GroundTruthReader(std::string filename) :
	_filename(filename) {

	registerOutput(_groundTruth, "ground truth");
}

void
GroundTruthReader::updateOutputs() {

	std::string number = "0123456789.eE-+";

	std::ifstream in(_filename.c_str());

	_groundTruth->clear();

	while (!in.eof() && in.good()) {

		std::string line = readline(in);

		if (line != "") {

			std::vector<double> f;

			size_t i = line.find_first_of(number);
			size_t end = line.find('#');

			while (i < end) {

				size_t j = line.find_first_not_of(number, i);
				_groundTruth->push_back(boost::lexical_cast<double>(line.substr(i, j - i)));

				i = line.find_first_of(number, j);
			}
		}
	}
}
