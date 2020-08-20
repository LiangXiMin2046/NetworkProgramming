#ifndef NETPROGRAMMING_CHATROOM_SERVER_SERVER_H
#define NETPROGRAMMING_CHATROOM_SERVER_SERVER_H

#include "/xmuduo/muduo/net/TcpServer.h"
#include "/NetworkProgramming/chatroom/codec.h"

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <set>

class ChatServer : boost::noncopyable
{
public:
	ChatServer(muduo::net::EventLoop* loop,
			   const muduo::net::InetAddress& peerAddr);

	void start();

private:
	void onConnection(const muduo::net::TcpConnectionPtr& conn);

	void onStringCallback(const muduo::net::TcpConnectionPtr& conn,
                          const muduo::string& msg,
						   muduo::Timestamp time);
	
	typedef std::set<muduo::net::TcpConnectionPtr> ConnectionList;
	muduo::net::TcpServer server_;
	muduo::net::EventLoop *loop_;
	LengthHeaderCodec codec_;
	ConnectionList connections_;
};

#endif
