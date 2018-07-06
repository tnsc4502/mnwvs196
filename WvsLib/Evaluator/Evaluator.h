#pragma once
#include "exprtk.hpp"
#include <vector>
#include <string>


class Evaluator
{
	exprtk::parser<double> m_parser;
	exprtk::expression<double> m_expression;
	exprtk::symbol_table<double> m_symbol_table;

public:
	Evaluator(double& ref, const std::string& evalStr);
	~Evaluator();

	//註冊時的變數改變了，取得新值
	double Eval();

	//整個重算
	static double Eval(const std::string& evalStr, const std::string& variableName, double& var);
};

