#ifndef NETWORKPROGRAMMING_FILETRANSFER_SERVER_SERVER_H
#define NETWORKPROGRAMMING_FILETRANSFER_SERVER_SERVER_H

#include "/xmuduo/muduo/net/TcpServer.h"

#include <stdio.h>
#include <boost/shared_ptr.hpp>

class DownloadServer
{
public:
	DownloadServer(muduo::net::EventLoop* loop,
				   const muduo::net::InetAddress addr,
				   muduo::string fileLocation);
	
	void start();
private:
	void onConnection(const muduo::net::TcpConnectionPtr& conn);
	void onMessage(const muduo::net::TcpConnectionPtr& conn,
				   muduo::net::Buffer* buf,
				   muduo::Timestamp time);
	void writeCompleteCallback(const muduo::net::TcpConnectionPtr& conn);

	muduo::net::TcpServer server_;
	muduo::net::EventLoop* loop_;
	muduo::string fileLocation_;
};

typedef boost::shared_ptr<FILE> FilePtr;
 
#endif
