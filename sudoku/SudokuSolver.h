#ifndef SODUKU_SOLVER_H
#define SODUKU_SOLVER_H

#include <boost/noncopyable.hpp>
#include <muduo/base/StringPiece.h>

class SudokuSolver : boost::noncopyable
{
public:
	SudokuSolver();
	std::string solve(const std::string& data);
private:
	int row[9][9],col[9][9],blocks[9][9],pre[9][9];
	char board[9][9];
	void dfs(int r,int c);
	void preProcess(const std::string& data);
	void getAnswer();
	std::string ans_;
	bool get_;
};

#endif
