#ifndef NETWORKPROGRAMMING_ROUNDTRIP_SERVER_SERVER_H
#define NETWORKPROGRAMMING_ROUNDTRIP_SERVER_SERVER_H

#include "/xmuduo/muduo/net/TcpServer.h"

#include <boost/noncopyable.hpp>

class RTTServer : public boost::noncopyable
{
public:

	RTTServer(muduo::net::EventLoop* loop,
              const muduo::net::InetAddress& addr);
	void start();

private:
	void onConnectionCallback(const muduo::net::TcpConnectionPtr& conn);
	void onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time);
	muduo::net::EventLoop *loop_;
	muduo::net::TcpServer server_;

	const static int frameLen = 3 * sizeof(int64_t);
};

#endif
