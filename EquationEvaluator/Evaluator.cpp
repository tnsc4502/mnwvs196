#include "Evaluator.h"
#include "exprtk.hpp"

Evaluator::Evaluator()
{
}


Evaluator::~Evaluator()
{
}

double Evaluator::Eval(const std::string & evalStr, const std::string & variableName, double & var)
{
	exprtk::parser<double> parser;
	exprtk::expression<double> expression;
	exprtk::symbol_table<double> symbol_table;
	symbol_table.add_variable("x", var, false);
	symbol_table.add_function("u", ceil);
	symbol_table.add_function("d", floor);

	expression.register_symbol_table(symbol_table);
	return parser.compile(evalStr, symbol_table).value();
}
