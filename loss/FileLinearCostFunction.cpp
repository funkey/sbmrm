#include <fstream>
#include <boost/lexical_cast.hpp>

#include <util/files.h>
#include "FileLinearCostFunction.h"

FileLinearCostFunction::FileLinearCostFunction(std::string filename) {

	std::string number = "0123456789.eE-+";

	std::ifstream in(filename.c_str());

	bool constantSet = false;

	_l.clear();

	while (!in.eof() && in.good()) {

		std::string line = readline(in);

		if (line != "") {

			std::vector<double> f;

			size_t i = line.find_first_of(number);
			size_t end = line.find('#');

			while (i < end) {

				size_t j = line.find_first_not_of(number, i);

				double value = boost::lexical_cast<double>(line.substr(i, j - i));

				if (!constantSet) {

					_c = value;
					constantSet = true;

				} else {

					_l.push_back(value);
				}

				i = line.find_first_of(number, j);
			}
		}
	}
}
