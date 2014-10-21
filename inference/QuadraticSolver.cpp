#include <util/Logger.h>
#include <util/foreach.h>
#include <util/helpers.hpp>
#include "QuadraticSolver.h"

static logger::LogChannel quadraticsolverlog("quadraticsolverlog", "[QuadraticSolver] ");

QuadraticSolver::QuadraticSolver(const QuadraticSolverBackendFactory& factory) :
		_constraintsAdded(0),
		_needReset(true) {

	registerInput(_objective, "objective");
	registerInput(_linearConstraints, "linear constraints");
	registerInput(_parameters, "parameters");
	registerOutput(_solution, "solution");

	_objective.registerBackwardCallback(&QuadraticSolver::onModified, this);
	_linearConstraints.registerBackwardCallback(&QuadraticSolver::onModified, this);
	_parameters.registerBackwardCallback(&QuadraticSolver::onModified, this);

	_linearConstraints.registerBackwardCallback(&QuadraticSolver::onConstraintAdded, this);

	_solver = factory.createQuadraticSolverBackend();
}

QuadraticSolver::~QuadraticSolver() {

	delete _solver;
}

void
QuadraticSolver::updateOutputs() {

	updateQuadraticProgram();

	solve();

	_constraintsAdded = 0;
	_needReset = false;
}

void
QuadraticSolver::onConstraintAdded(const ConstraintAdded& /*signal*/) {

	LOG_DEBUG(quadraticsolverlog) << "a single constraint was added" << std::endl;

	_constraintsAdded++;
}

void
QuadraticSolver::onModified(const pipeline::Modified& /*signal*/) {

	LOG_DEBUG(quadraticsolverlog) << "an input changed entirely, reset QP on next update" << std::endl;

	_needReset = true;
}

void
QuadraticSolver::updateQuadraticProgram() {

	if (_needReset) {

		if (_parameters)
			_solver->initialize(
					getNumVariables(),
					_parameters->getDefaultVariableType(),
					_parameters->getSpecialVariableTypes());
		else
			_solver->initialize(
					getNumVariables(),
					Continuous);

		_solver->setObjective(*_objective);

		_solver->setConstraints(*_linearConstraints);

	// only constraints got added
	} else {

		unsigned int numConstraints = _linearConstraints->size();

		// add all the new constraints
		for (unsigned int i = 0; i < _constraintsAdded; i++)
			_solver->addConstraint((*_linearConstraints)[numConstraints - 1 - i]);
	}
}

void
QuadraticSolver::solve() {

	double value;

	std::string message;

	if (_solver->solve(*_solution, value, message)) {

		LOG_DEBUG(quadraticsolverlog) << message << std::endl;

	} else {

		LOG_ERROR(quadraticsolverlog) << "error: " << message << std::endl;
	}

	LOG_ALL(quadraticsolverlog) << "solution: " << _solution->getVector() << std::endl;
}

unsigned int
QuadraticSolver::getNumVariables() {

	unsigned int numVars = 0;

	// number of vars in the objective
	numVars = std::max<unsigned int>(numVars, _objective->getCoefficients().size());

	typedef std::pair<std::pair<unsigned int, unsigned int>, double> quad_coef_type;
	foreach (const quad_coef_type& pair, _objective->getQuadraticCoefficients())
		numVars = std::max(numVars, std::max(pair.first.first + 1, pair.first.second + 1));

	// number of vars in the constraints
	typedef std::pair<unsigned int, double> lin_coef_type;
	foreach (const LinearConstraint& constraint, *_linearConstraints)
		foreach (const lin_coef_type& pair, constraint.getCoefficients())
			numVars = std::max(numVars, pair.first + 1);

	return numVars;
}
