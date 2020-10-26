#ifndef SUDOKU_SERVER_H
#define SUDOKU_SERVER_H

#include <muduo/net/TcpServer.h>
#include <boost/noncopyable.hpp>

class SudokuServer : boost::noncopyable
{
public:
	SudokuServer(muduo::net::EventLoop* loop,
                 const muduo::net::InetAddress& listenAddr,
                 int threadNum);
	void start();
private:
	void onConnection(const muduo::net::TcpConnectionPtr& conn);
	void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);
	std::string solve(const std::string& data);
	muduo::net::EventLoop* loop_;
	muduo::net::TcpServer server_;
};

#endif
