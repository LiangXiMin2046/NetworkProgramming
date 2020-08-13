#ifndef TASKS_SIMPLES_ECHO_H
#define TASKS_SIMPLES_ECHO_H

#include "/xmuduo/muduo/net/TcpServer.h"

//RFC 862
class EchoServer
{
public:
	EchoServer(muduo::net::EventLoop* loop,
			   const muduo::net::InetAddress& listenAddr);

	void start(); //calls server_.start();
private:
	void onConnection(const muduo::net::TcpConnectionPtr& conn);
	void onMessage(const muduo::net::TcpConnectionPtr& conn,
				   muduo::net::Buffer* buf,
				   muduo::Timestamp time);
	
	muduo::net::EventLoop* loop_;
	muduo::net::TcpServer server_;
};

#endif
