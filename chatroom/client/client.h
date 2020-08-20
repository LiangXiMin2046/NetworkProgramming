#ifndef NETPROGRAMMING_CHATROOM_CLIENT_CLIENT_H
#define NETPROGRAMMING_CHATROOM_CLIENT_CLIENT_H

#include "/NetworkProgramming/chatroom/codec.h"
#include "/xmuduo/muduo/base/Mutex.h"
#include "/xmuduo/muduo/base/StringPiece.h"
#include "/xmuduo/muduo/net/TcpClient.h"

#include <boost/noncopyable.hpp>

class ChatClient : boost::noncopyable
{
public:
	ChatClient(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& peerAddr);

	void connect();
	void disconnect();
	void write(const muduo::StringPiece& msg);

private:
	void onConnectionCallback(const muduo::net::TcpConnectionPtr& conn);
	void onStringCallback(const muduo::net::TcpConnectionPtr& conn,
                          const muduo::string& msg,
						  muduo::Timestamp time);

	muduo::MutexLock mutex_;
	muduo::net::TcpClient client_;
	muduo::net::EventLoop* loop_;
	muduo::net::TcpConnectionPtr connection_;
	LengthHeaderCodec codec_;
};

#endif
