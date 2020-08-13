#ifndef NETWORKPROGRAMMING_SIMPLES_CHARGEN_H
#define NETWORKPROGRAMMING_SIMPLES_CHARGEN_H

#include "/xmuduo/muduo/net/TcpServer.h"
#include "/xmuduo/muduo/base/Timestamp.h"

//RFC 842
class ChargenServer
{
public:
	ChargenServer(muduo::net::EventLoop* loop,const muduo::net::InetAddress& listenAddr);

	void start();	
private:
	void initMsg();
	void onConnection(const muduo::net::TcpConnectionPtr&);
	void onMessage(const muduo::net::TcpConnectionPtr& conn,
				   muduo::net::Buffer* inputBuffer,
				   muduo::Timestamp time);
	void printThrough();
	void writeCompleteCallback(const muduo::net::TcpConnectionPtr& conn);
	muduo::net::TcpServer server_;
	muduo::string msg_;
	int64_t count_;
	muduo::Timestamp startTime_;
	muduo::net::EventLoop* loop_;

};

#endif

