#include "Evaluator.h"

Evaluator::Evaluator(double& var, const std::string& evalStr)
{
	m_symbol_table.add_variable("x", var);
	m_symbol_table.add_function("u", ceil);
	m_symbol_table.add_function("d", floor);

	m_expression.register_symbol_table(m_symbol_table);
	m_parser.compile(evalStr, m_expression);
}

Evaluator::~Evaluator()
{

}

double Evaluator::Eval()
{
	return m_expression.value();
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
