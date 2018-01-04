#pragma once
#include <vector>
#include <string>

class Evaluator
{
public:
	Evaluator();
	~Evaluator();

	double Eval(const std::string& evalStr, const std::string& variableName, double& var);
};

