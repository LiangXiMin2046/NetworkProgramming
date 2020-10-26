#include "SudokuSolver.h"

#include <string.h>

SudokuSolver::SudokuSolver()
    :  get_(false)
{
}

void SudokuSolver::preProcess(const std::string& data)
{
	::memset(pre,0,sizeof pre);
	::memset(row,0,sizeof row);
	::memset(col,0,sizeof col);
	::memset(blocks,0,sizeof blocks);
	::memset(board,'0',sizeof board);
	get_ = false;

	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9; j++)
		{	
			board[i][j] = data[i*9+j];
			if(board[i][j] != '0')
			{
				pre[i][j] = 1;
				row[i][board[i][j]-'1'] = 1;
				col[j][board[i][j]-'1'] = 1;
				blocks[i/3*3+j/3][board[i][j]-'1'] = 1;
			}
		}	
	}	
}

void SudokuSolver::dfs(int r,int c)
{
	if(r == 9)
	{
		ans_.clear();
		for(int i = 0; i < 9; i++)
		{
			for(int j = 0; j < 9; j++)
			{
				ans_ += board[i][j];
				ans_ += " ";
			}
			ans_ += "\r\n";
		}
		get_ = true;
		return;	
	}

	if(c == 9)
	{
		dfs(r+1,0);
		return;
	}
	//std::cout << r << " " << c << std::endl;
	if(pre[r][c])
	{
		dfs(r,c+1);
		return;
	}

	for(char i = '1'; i <= '9'; i++) if(!get_ && !row[r][i-'1'] && !col[c][i-'1'] && !blocks[r/3*3+c/3][i-'1'])
	{
		board[r][c] = i;
		row[r][i-'1'] = 1;
		col[c][i-'1'] = 1;
		blocks[r/3*3+c/3][i-'1'] = 1;
		dfs(r,c+1);	
		row[r][i-'1'] = 0;
		col[c][i-'1'] = 0;
		blocks[r/3*3+c/3][i-'1'] = 0;
	}
}

std::string SudokuSolver::solve(const std::string& data)
{
	preProcess(data);
	dfs(0,0);
	return ans_;	
}
