/**
 * structured bmrm main file. Initializes all objects.
 */

#include <iostream>
#include <pipeline/Process.h>
#include <pipeline/Value.h>
#include <util/ProgramOptions.h>
#include <util/Logger.h>

#include <bundle/BundleMethod.h>

using namespace logger;

util::ProgramOption optionLabelsFile(
		util::_long_name        = "labelsFile",
		util::_description_text = "File containing the ground truth labels.",
		util::_default_value    = "labels.txt");

util::ProgramOption optionFeaturesFile(
		util::_long_name        = "featuresFile",
		util::_description_text = "File containing the features of the training sample.",
		util::_default_value    = "features.txt");

util::ProgramOption optionConstraintsFile(
		util::_long_name        = "constraintsFile",
		util::_description_text = "File containing the constraints on the labels.",
		util::_default_value    = "constraints.txt");

void quadraticTest(std::vector<double> x, double& y, std::vector<double>& gradient) {

	LOG_USER(out) << "computing L(w) at " << x[0] << std::endl;

	y = 10*x[0] + x[0]*x[0];
	gradient[0] = 10 + 2*x[0];

	LOG_USER(out) << "y = " << y << std::endl;
	LOG_USER(out) << "∂y/∂x = " << gradient[0] << std::endl;
}

int main(int optionc, char** optionv) {

	try {

		/********
		 * INIT *
		 ********/

		// init command line parser
		util::ProgramOptions::init(optionc, optionv);

		// init logger
		LogManager::init();

		LOG_USER(out) << "[main] starting..." << std::endl;

		BundleMethod bundleMethod(quadraticTest, 1, 1.0, 0.0001);

		std::vector<double> min = bundleMethod.optimize();

		LOG_USER(out) << "[main] optimial value of 10x + x² is at " << min[0] << std::endl;

	} catch (Exception& e) {

		handleException(e, std::cerr);
	}
}

