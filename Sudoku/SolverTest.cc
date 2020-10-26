#include "SudokuSolver.h"

#include <iostream>

using namespace std;

int main(void)
{
	//std::string data = "000000010400000000020000000000050407008000300001090000300400200050100000000806000";
	std::string data = "000010054800000000000000000650400000000002730000000000210000800700000300000350000";
	SudokuSolver solver;
	std::string result = solver.solve(data);
	cout << result << endl;
	return 0;
}
