#include "chargen.h"
#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/EventLoop.h"

#include <fcntl.h>
#include <stdio.h>
#include <boost/bind.hpp>

ChargenServer::ChargenServer(muduo::net::EventLoop* loop,const::muduo::net::InetAddress& listenAddr)
	:	count_(0),
		loop_(loop),
		server_(loop,listenAddr,"ChargenServer"),
		startTime_(0)
{
	server_.setConnectionCallback(boost::bind(&ChargenServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&ChargenServer::onMessage,this,_1,_2,_3));
	server_.setWriteCompleteCallback(boost::bind(&ChargenServer::writeCompleteCallback,this,_1));
	initMsg();
	loop->runEvery(3.0,boost::bind(&ChargenServer::printThrough,this));
}

void ChargenServer::start()
{
	server_.start();
}

void ChargenServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection between" << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << "is " << (conn->connected() ? "up" : "down");
	
	muduo::Timestamp now(muduo::Timestamp::now());
	startTime_.swap(now);
	if(conn->connected())
	{
		conn->send(msg_);
	}
}

void ChargenServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
			   muduo::net::Buffer* inputBuffer,
			   muduo::Timestamp time)
{
	inputBuffer->retrieveAll();
}

void ChargenServer::writeCompleteCallback(const muduo::net::TcpConnectionPtr& conn)
{
	count_ += msg_.size();
	conn->send(msg_);	
}

void ChargenServer::printThrough()
{
	int64_t currentCount = count_;
	muduo::Timestamp endTime(muduo::Timestamp::now());	
	double secs = muduo::timeDifference(endTime,startTime_);
	int fd = open("ThroughRecord",O_WRONLY | O_CREAT | O_APPEND,0644);
	char buf[128];
	int numBytes = snprintf(buf,sizeof buf,"the through out is %4.2f mb per second,time at : %s\n",count_/(secs*1024*1024),endTime.toFormattedString().c_str());
	write(fd,buf,numBytes);
	close(fd);
	count_ -= currentCount;
	startTime_.swap(endTime);
}

void ChargenServer::initMsg()
{
	muduo::string tem;
	for(char c = 32; c <= 126; ++c)
	{
		tem += c;
	}
	tem += tem;
	for(int i = 0; i < 95; ++i)
		msg_ += tem.substr(i,72)+'\n'; 
}
