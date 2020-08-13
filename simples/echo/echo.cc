#include "echo.h"

#include "/xmuduo/muduo/base/Timestamp.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <boost/bind.hpp>

EchoServer::EchoServer(muduo::net::EventLoop* loop,
					   const muduo::net::InetAddress& listenAddr)
	:	loop_(loop),
		server_(loop_,listenAddr,"EchoServer")
{
	server_.setConnectionCallback(boost::bind(&EchoServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&EchoServer::onMessage,this,_1,_2,_3));
}

void EchoServer::start()
{
	server_.start();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "EchoServer - " << conn -> peerAddress().toIpPort() << "->"	
			 << conn->localAddress().toIpPort() << "is"
			 << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		conn->send(muduo::Timestamp::now().toFormattedString()+'\n');		
	}
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
						   muduo::net::Buffer* buf,
						   muduo::Timestamp time)
{
	muduo::string msg(buf->retrieveAllAsString());
	LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
			 << "data received at " << time.toString();
	conn->send(msg);
}
