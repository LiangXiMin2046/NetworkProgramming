#include "client.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <assert.h>
#include <boost/bind.hpp>

ReceiveClient::ReceiveClient(muduo::net::EventLoop* loop,const muduo::net::InetAddress& serverAddr)
	:	loop_(loop),
		client_(loop,serverAddr,"ReceiveClient")
{
	client_.setConnectionCallback(boost::bind(&ReceiveClient::onConnectionCallback,this,_1));
	client_.setMessageCallback(boost::bind(&ReceiveClient::onMessageCallback,this,_1,_2,_3));
	client_.enableRetry();
	fp_ = ::fopen("ReceiveFile","ab");
	assert(fp_);
}

ReceiveClient::~ReceiveClient()
{
	::fclose(fp_);
}

void ReceiveClient::connect()
{
	client_.connect();
}

void ReceiveClient::onConnectionCallback(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection between " << conn->peerAddress().toIpPort() << " is " << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		connection_ = conn;
	}
	else
	{
		client_.disconnect();
		connection_.reset();
		loop_->quit();
	}
}

void ReceiveClient::onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
									  muduo::net::Buffer* buf,
									  muduo::Timestamp time)
{
	LOG_INFO << "new data is transferd";	
	::fwrite(buf->peek(),1,buf->readableBytes(),fp_);
	buf->retrieveAll();
} 
