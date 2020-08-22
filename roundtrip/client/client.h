#ifndef NETWORKPROGRAMMING_ROUNDTRIP_CLIENT_CLIENT_H
#define NETWORKPROGRAMMING_ROUNDTRIP_CLIENT_CLIENT_H

#include "/xmuduo/muduo/net/TcpClient.h"

#include <boost/noncopyable.hpp>

class RTTClient : public boost::noncopyable
{
public:
	RTTClient(muduo::net::EventLoop* loop,
              const muduo::net::InetAddress& addr);

	void connect();
private:
	void onConnectionCallback(const muduo::net::TcpConnectionPtr& conn);
	void onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp receiveTime);
	void sendMessage();

	muduo::net::TcpClient client_;
	muduo::net::EventLoop* loop_;
	muduo::net::TcpConnectionPtr connection_;

	const static int frameLen_ = 3 * sizeof(int64_t);
};

#endif
