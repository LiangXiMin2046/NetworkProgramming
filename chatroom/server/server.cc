#include "server.h"
#include "/xmuduo/muduo/net/EventLoop.h"

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>

ChatServer::ChatServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& peerAddr)
	:	loop_(loop),
		server_(loop,peerAddr,"CharServer"),
		codec_(boost::bind(&ChatServer::onStringCallback,this,_1,_2,_3))
{
	server_.setConnectionCallback(boost::bind(&ChatServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage,&codec_,_1,_2,_3));
}

void ChatServer::start()
{
	server_.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection to " << conn->peerAddress().toIpPort() << "is" << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		connections_.insert(conn);
		codec_.send(get_pointer(conn),"welcome to our chatroom!\n");
	}
	else
	{
		connections_.erase(conn);	
	}
}
void ChatServer::onStringCallback(const muduo::net::TcpConnectionPtr& conn,
                                  const muduo::string& msg,
                                  muduo::Timestamp time)
{
	for(ConnectionList::const_iterator it = connections_.begin(); it != connections_.end(); ++it)
	{
		codec_.send(get_pointer(*it),msg);
	}
}

