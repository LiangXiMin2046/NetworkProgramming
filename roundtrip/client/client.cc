#include "client.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <boost/bind.hpp>
#include <string.h>
#include <stdio.h>

RTTClient::RTTClient(muduo::net::EventLoop* loop,
                     const muduo::net::InetAddress& addr)
	:	loop_(loop),
		client_(loop,addr,"RTTClient")
{
	client_.setConnectionCallback(boost::bind(&RTTClient::onConnectionCallback,this,_1));
	client_.setMessageCallback(boost::bind(&RTTClient::onMessageCallback,this,_1,_2,_3));
	client_.enableRetry();
	loop->runEvery(2.0,boost::bind(&RTTClient::sendMessage,this));
}

void RTTClient::connect()
{
	client_.connect();
}
void RTTClient::onConnectionCallback(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection to " << conn->peerAddress().toIpPort() << " is " << (conn->connected() ? "up" : "down");
	if(conn->connected())
	{
		conn->setTcpNoDelay(true);
		connection_ = conn;
	}
	else
	{
		connection_.reset();
	}
}

void RTTClient::sendMessage()
{
	if(connection_->connected())
	{
		muduo::Timestamp now(muduo::Timestamp::now());
		int64_t message[3];
		::memset(message,0,frameLen_);
		message[0] = now.microSecondsSinceEpoch();
		connection_->send(message,frameLen_);
	}	
}

void RTTClient::onMessageCallback(const muduo::net::TcpConnectionPtr& conn,
                                  muduo::net::Buffer* buf,
                                  muduo::Timestamp receiveTime)
{
	int64_t messageTime[3];
	while(buf->readableBytes() >= frameLen_)
	{
		::memcpy(messageTime,buf->peek(),frameLen_);
		buf->retrieve(frameLen_);
		int64_t now = receiveTime.microSecondsSinceEpoch();
		printf("round trip time is : %lld us\n",
          (now - messageTime[0] )- (messageTime[2] - messageTime[1]));
		double offset = static_cast<double>(now + messageTime[0] - 
          messageTime[1] - messageTime[2])/ 2; 
		printf("clock offset between client and server is %.2lf us\n",offset);
		//double oneWayTimeTo = static_cast<double>(messageTime[1]) + offset - messageTime[0]; 
		//printf("oneWayTimeTo is %.2lf\n",oneWayTimeTo);
		double oneWayTimeBack = static_cast<double>(now) - offset - messageTime[2];
		printf("oneWayTimeBack is %.2lf\n",oneWayTimeBack);
	}
}
