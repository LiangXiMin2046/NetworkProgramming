#include "server.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <boost/bind.hpp>
#include <assert.h>

EchoServer::EchoServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& addr,
                       int listSize)
	:	loop_(loop),
		server_(loop,addr,"EchoServer")
{
	server_.setConnectionCallback(boost::bind(&EchoServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&EchoServer::onMessage,this,_1,_2,_3));
	connectionBuckets_.resize(listSize);
	loop->runEvery(1.0,boost::bind(&EchoServer::onTimer,this));
}

void EchoServer::start()
{
	server_.start();
}

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection between " << conn->peerAddress().toIpPort() << "is" << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		EntryPtr ptr(new Entry(conn));
		connectionBuckets_.back().insert(ptr);
		WeakEntryPtr weakPtr(ptr);
		conn->setContext(weakPtr);
	}
	else
	{
		assert(!conn->getContext().empty());
		WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
		LOG_INFO << "Entry use_count = " << weakEntry.use_count();		
	}
}

void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time)
{
	muduo::string msg(buf->retrieveAllAsString());
	conn->send(msg);

	assert(!conn->getContext().empty());
	WeakEntryPtr weakEntry(boost::any_cast<WeakEntryPtr>(conn->getContext()));
	EntryPtr entry(weakEntry.lock());
	if(entry)
		connectionBuckets_.back().insert(entry);
}

void EchoServer::onTimer()
{
	connectionBuckets_.push_back(Bucket());	
}
