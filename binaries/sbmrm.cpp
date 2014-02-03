/**
 * structured bmrm main file. Initializes all objects.
 */

#include <iostream>
#include <pipeline/Process.h>
#include <pipeline/Value.h>
#include <util/ProgramOptions.h>
#include <util/Logger.h>
#include <util/helpers.hpp>

#include <bundle/BundleMethod.h>
#include <loss/HammingCostFunction.h>
#include <loss/SoftMarginLoss.h>
#include <loss/io/FeaturesReader.h>
#include <loss/io/GroundTruthReader.h>
#include <inference/io/ConstraintsReader.h>

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

void quadraticTest(const std::vector<double>& x, double& y, std::vector<double>& gradient) {

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

		pipeline::Process<ConstraintsReader> constraintsReader(optionConstraintsFile.as<std::string>());
		pipeline::Process<FeaturesReader>    featuresReader(optionFeaturesFile.as<std::string>());
		pipeline::Process<GroundTruthReader> groundTruthReader(optionLabelsFile.as<std::string>());

		// get them read
		pipeline::Value<LinearConstraints> constraints = constraintsReader->getOutput();
		LinearConstraints c = *constraints;

		pipeline::Value<Features> features = featuresReader->getOutput();
		LOG_DEBUG(out) << "[main] read " << features->numFeatureVectors() << " feature vectors of size " << features->numFeatures() << std::endl;

		for (unsigned int i = 0; i < features->numFeatureVectors(); i++)
			LOG_DEBUG(out) << "[main] \t" << features->getFeatureVector(i) << std::endl;

		pipeline::Value<std::vector<double> > groundTruth = groundTruthReader->getOutput();
		LOG_DEBUG(out) << "[main] ground truth labeling is: " << *groundTruth << std::endl;

		HammingCostFunction costs(*groundTruth);

		SoftMarginLoss loss(costs, constraints, features, groundTruth);

		//BundleMethod bundleMethod(quadraticTest, 1, 1.0, 0.0001);
		BundleMethod::callback_t callback = boost::bind(&SoftMarginLoss::valueAndGradient, &loss, _1, _2, _3);
		BundleMethod bundleMethod(callback, features->numFeatures(), 1.0, 0.0001);

		std::vector<double> w = bundleMethod.optimize();

		LOG_USER(out) << "[main] optimial w is " << w << std::endl;

	} catch (Exception& e) {

		handleException(e, std::cerr);
	}
}

