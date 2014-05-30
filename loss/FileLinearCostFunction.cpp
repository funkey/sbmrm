#include <fstream>
#include <boost/lexical_cast.hpp>

#include <util/files.h>
#include "FileLinearCostFunction.h"

FileLinearCostFunction::FileLinearCostFunction(std::string filename) {

	std::string number = "0123456789.eE-+";

	std::ifstream in(filename.c_str());

	_l.clear();

	unsigned int lineNumber = 1;

	while (!in.eof() && in.good()) {

		// Read line
		std::string line = readline(in);

		// Process line
		if (line != "") {

			// Is this the first line? In this case we expect information about the number of variables.
			if (lineNumber == 1) {

				if (line.find("numVar") != std::string::npos) { 	// Line contains information about the number of variables.
					
					size_t beginning = line.find_first_of(number);
					size_t end = line.find('#');
					double numVar = boost::lexical_cast<double>(line.substr(beginning, end - beginning));

					// initiate vector with zeros, this enables sparse representation of the variables.
					for ( int varNum = 0; varNum < numVar; varNum++ ) {
						_l[varNum] = 0;
					}

				}
				else {
					std::cout << "Could not read variables since the number of variables was not specified in the first line." << std::endl;
					break;
				}
			}
			else {
				
				// Does this line specify a variable or the constant?
				if (line.find("constant") != std::string::npos) { 	// constant

					// Read constant
					size_t beginning = line.find_first_of(number);
					size_t end = line.find('#');
					double value = boost::lexical_cast<double>(line.substr(beginning, end - beginning));

					// Save
					_c = value;
				}
				else {							// variable
					// Read variable
					// The value is assumed to start after the first blank.
					// An improvement could be made here to make the representation 'sparse'.

					// Read variable number
					size_t beginningVarNum = line.find_first_of(number);
					size_t endVarNum = line.find_first_of(" ") -1;
					double varNum = boost::lexical_cast<double>(line.substr(beginningVarNum, endVarNum - beginningVarNum));

					size_t beginningValue = line.find_first_of(" ") + 1;
					size_t endValue = line.find('#');
					double value = boost::lexical_cast<double>(line.substr(beginningValue, endValue - beginningValue));

					// Save
					_l[varNum] = value;
				}
			}
		
			lineNumber++;
		}
	}
}
