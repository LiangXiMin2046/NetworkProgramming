#ifndef NETWORKINGPROGRAMMING_FILETRANSFER_CLIENT_CLIENT_H
#define NETWORKINGPROGRAMMING_FILETRANSFER_CLIENT_CLIENT_H

#include "/xmuduo/muduo/net/TcpClient.h"

#include <stdio.h>

class ReceiveClient
{
public:
	ReceiveClient(muduo::net::EventLoop* loop,
				  const muduo::net::InetAddress& serverAddr);
	
	~ReceiveClient();
	void connect();
private:
	void onConnectionCallback(const muduo::net::TcpConnectionPtr& conn);
	void onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
						   muduo::net::Buffer* buf,
						   muduo::Timestamp time);

	muduo::net::TcpConnectionPtr connection_; 
	muduo::net::EventLoop* loop_;
	muduo::net::TcpClient client_;
	FILE* fp_;
};
 
#endif
