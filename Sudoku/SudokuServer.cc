#include "SudokuServer.h"
#include "SudokuSolver.h"

#include <boost/bind.hpp>

SudokuServer::SudokuServer(muduo::net::EventLoop* loop,const muduo::net::InetAddress& listenAddr,int threadNum)
  :  loop_(loop),
     server_(loop_,listenAddr,"SudokuServer")
{
	server_.setThreadNum(threadNum);
	server_.setConnectionCallback(boost::bind(&SudokuServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&SudokuServer::onMessage,this,_1,_2,_3));
}

void SudokuServer::start()
{
	server_.start();
}

void SudokuServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		conn->send("Sudoku server\r\n");
	}
}

void SudokuServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                             muduo::net::Buffer* buf,
                             muduo::Timestamp time)
{
	while(buf->readableBytes() >= 2)
	{
		if(buf->findCRLF())
		{
			std::string data = buf->retrieveAllAsString();
			data.pop_back();
			data.pop_back();
			std::string result;
			conn->send("calculating...\r\n");
			result = solve(data);
			conn->send(result);
			conn->send("finished\r\n");
		}
	}
}

std::string SudokuServer::solve(const std::string& data)
{
	std::string result,calResult;
	size_t pos;
	if((pos = data.find(":")) != std::string::npos)
	{
		result += data.substr(0,pos) + ":\r\n";
	}

	SudokuSolver solver;	
	calResult = solver.solve(data.substr(pos+1));
	if(calResult.empty())
		result += "No solution";
	else
		result += calResult;
	return result;
}
