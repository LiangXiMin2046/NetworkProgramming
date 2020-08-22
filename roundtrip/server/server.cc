#include "server.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"
//#include "/xmuduo/muduo/base/Timestamp.h"

#include <string.h>
#include <boost/bind.hpp>

RTTServer::RTTServer(muduo::net::EventLoop* loop,
                     const muduo::net::InetAddress& addr)
	:	loop_(loop),
		server_(loop,addr,"RTTServer")
{
	server_.setConnectionCallback(boost::bind(&RTTServer::onConnectionCallback,this,_1));
	server_.setMessageCallback(boost::bind(&RTTServer::onMessageCallback,this,_1,_2,_3));
}

void RTTServer::start()
{
	server_.start();
}

void RTTServer::onConnectionCallback(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection between " << conn->peerAddress().toIpPort() << " is " << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		conn->setTcpNoDelay(true);
	}
}

void RTTServer::onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
                       muduo::net::Buffer* buf,
                       muduo::Timestamp time)
{
	int64_t message[3];
	while(buf->readableBytes() >= frameLen)
	{
		::memcpy(message,buf->peek(),frameLen);
		buf->retrieve(frameLen);
		message[1] = time.microSecondsSinceEpoch();
		muduo::Timestamp now(muduo::Timestamp::now());
		message[2] = now.microSecondsSinceEpoch();
		conn->send(message,frameLen);	
	}
}
