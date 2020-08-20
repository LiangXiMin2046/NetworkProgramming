#include "client.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdio.h>

ChatClient::ChatClient(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& addr)
	:	loop_(loop),
        client_(loop,addr,"chatclient"),
		mutex_(),
        codec_(boost::bind(&ChatClient::onStringCallback,this,_1,_2,_3))
{
	client_.setConnectionCallback(boost::bind(&ChatClient::onConnectionCallback,this,_1));
	client_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage,&codec_,_1,_2,_3));
}

void ChatClient::connect()
{
	client_.connect();
}

void ChatClient::disconnect()
{
	client_.disconnect();
}

void ChatClient::onConnectionCallback(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection to server " << conn->peerAddress().toIpPort() << "is" << (conn->connected() ? "up" : "down");

	muduo::MutexLockGuard lock(mutex_);
	if(conn->connected())
	{
		connection_ = conn;
	}
	else
	{
		connection_.reset();
	}
}

void ChatClient::onStringCallback(const muduo::net::TcpConnectionPtr& conn,
                      const muduo::string& msg,
                      muduo::Timestamp time)
{
	printf("<<< %s\n",msg.c_str());
}

void ChatClient::write(const muduo::StringPiece& msg)
{
	muduo::MutexLockGuard lock(mutex_);
	if(connection_)
	{
		codec_.send(get_pointer(connection_),msg);
	}
} 
