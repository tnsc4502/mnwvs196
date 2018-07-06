#include "Evaluator.h"

Evaluator::Evaluator()
{
}


Evaluator::Evaluator(double& var, const std::string& evalStr)
{
	symbol_table.add_variable("x", var);
	symbol_table.add_function("u", ceil);
	symbol_table.add_function("d", floor);

	expression.register_symbol_table(symbol_table);
	parser.compile(evalStr, expression);
}

void Evaluator::Compile(const  std::string& str)
{
}

Evaluator::~Evaluator()
{

}

double Evaluator::Eval()
{
	//m_expression.register_symbol_table(symbol_table);
	return expression.value();
}

double Evaluator::Eval(const std::string & evalStr, const std::string & variableName, double & var)
{

	//m_expression.register_symbol_table(symbol_table);
	return 0;
}
