#pragma once
#include "exprtk.hpp"
#include <vector>
#include <string>


class Evaluator
{
	exprtk::parser<double> parser;
	exprtk::expression<double> expression;
	exprtk::symbol_table<double> symbol_table;
public:
	Evaluator();
	Evaluator(double& ref, const std::string& evalStr);
	~Evaluator();
	void Compile(const std::string& evalStr);
	double Eval();

	double Eval(const std::string& evalStr, const std::string& variableName, double& var);
};

