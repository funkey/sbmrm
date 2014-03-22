#include <fstream>

#include <util/Logger.h>
#include <util/files.h>
#include <util/helpers.hpp>
#include "FeaturesReader.h"

logger::LogChannel featuresreaderlog("featuresreaderlog", "[FeaturesReader] ");

FeaturesReader::FeaturesReader(std::string filename) :
	_filename(filename) {

	registerOutput(_features, "features");
}

void
FeaturesReader::updateOutputs() {

	std::string number = "0123456789.eE-+";

	std::ifstream in(_filename.c_str());

	_features->clear();

	while (!in.eof() && in.good()) {

		std::string line = readline(in);

		LOG_ALL(featuresreaderlog) << "reading line: " << line << std::endl;

		if (line != "") {

			std::vector<double> f;

			size_t i = line.find_first_of(number);
			size_t end = line.find('#');

			LOG_ALL(featuresreaderlog) << "end of line is at " << end << std::endl;
			LOG_ALL(featuresreaderlog) << "first number is at " << i << std::endl;

			while (i < end) {

				size_t j = line.find_first_not_of(number, i);
				LOG_ALL(featuresreaderlog) << "number stops at " << j << std::endl;
				f.push_back(boost::lexical_cast<double>(line.substr(i, j - i)));

				i = line.find_first_of(number, j);

				LOG_ALL(featuresreaderlog) << "next number is at " << i << std::endl;
			}

			LOG_ALL(featuresreaderlog) << "adding feature vector " << f << std::endl;

			if (f.size() > 0)
				_features->addFeatureVector(f);
		}
	}
}
