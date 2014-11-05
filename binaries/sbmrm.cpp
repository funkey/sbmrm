/**
 * structured bmrm main file. Initializes all objects.
 */

#include <iostream>
#include <fstream>
#include <pipeline/Process.h>
#include <pipeline/Value.h>
#include <util/ProgramOptions.h>
#include <util/Logger.h>
#include <util/helpers.hpp>

#include <bundle/BundleMethod.h>
#include <loss/HammingCostFunction.h>
#include <loss/FileLinearCostFunction.h>
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

util::ProgramOption optionWeightsOutFile(
		util::_long_name	= "weightsOutputFile",
		util::_description_text = "File the computet optimal weights are written to.",
		util::_default_value	= "weights.txt");

util::ProgramOption optionLinearCostsFile(
		util::_long_name        = "linearCostsFile",
		util::_description_text = "File with the values for a linear cost function. If not set, Hamming costs are used.");

util::ProgramOption optionRegularizerWeight(
		util::_long_name        = "regularizerWeight",
		util::_description_text = "The regularizer influence on the learning objective.",
		util::_default_value    = 1.0);

util::ProgramOption optionNormalizeFeatures(
		util::_long_name        = "normalizeFeatures",
		util::_description_text = "Normalize features, such that their absolute values is in the range [0,1].");

util::ProgramOption optionOptimizerGap(
		util::_long_name        = "optimizerGap",
		util::_description_text = "The optimality criterion for stopping the bundle method.",
		util::_default_value    = 1e-5);

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
		pipeline::Process<FeaturesReader>    featuresReader(optionFeaturesFile.as<std::string>(), optionNormalizeFeatures.as<bool>());
		pipeline::Process<GroundTruthReader> groundTruthReader(optionLabelsFile.as<std::string>());

		// get them read
		pipeline::Value<LinearConstraints>    constraints = constraintsReader->getOutput();
		pipeline::Value<Features>             features    = featuresReader->getOutput();
		pipeline::Value<std::vector<double> > groundTruth = groundTruthReader->getOutput();

		boost::shared_ptr<LinearCostFunction> costs;

		if (!optionLinearCostsFile)
			costs = boost::make_shared<HammingCostFunction>(*groundTruth);
		else
			costs = boost::make_shared<FileLinearCostFunction>(optionLinearCostsFile.as<std::string>());

		SoftMarginLoss loss(*costs, constraints, features, groundTruth);

		BundleMethod::callback_t callback = boost::bind(&SoftMarginLoss::valueAndGradient, &loss, _1, _2, _3);
		BundleMethod bundleMethod(callback, features->numFeatures(), optionRegularizerWeight, optionOptimizerGap);

		std::vector<double> w = bundleMethod.optimize();

		if (optionNormalizeFeatures)
			features->normalize(w);

		LOG_USER(out) << "[main] optimial w is " << w << std::endl;

		// write output
		std::ofstream wOutput;
		wOutput.open(optionWeightsOutFile.as<std::string>().c_str());
		for (unsigned int i = 0; i < w.size(); i++) {
			wOutput << w[i] << std::endl;
		}
		wOutput.close();

	} catch (Exception& e) {

		handleException(e, std::cerr);
	}
}

