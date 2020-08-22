#include "server.h"
#include "/xmuduo/muduo/base/Logging.h"
#include "/xmuduo/muduo/net/EventLoop.h"

#include <boost/bind.hpp>

const int writeSize_ = 1024;

DownloadServer::DownloadServer(muduo::net::EventLoop* loop,
							   const muduo::net::InetAddress addr,
							   muduo::string fileLocation)
	:	loop_(loop),
		fileLocation_(fileLocation),
		server_(loop,addr,"DownloadServer")
{
	server_.setConnectionCallback(boost::bind(&DownloadServer::onConnection,this,_1));
	server_.setMessageCallback(boost::bind(&DownloadServer::onMessage,this,_1,_2,_3));
	server_.setWriteCompleteCallback(boost::bind(&DownloadServer::writeCompleteCallback,this,_1));
}

void DownloadServer::start()
{
	server_.start();
}

void DownloadServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO << "the connection from " << conn->peerAddress().toIpPort() << " has " << (conn->connected() ? "up" : "down");

	if(conn->connected())
	{
		FILE *file = ::fopen(fileLocation_.c_str(),"rb");
		if(file == NULL)
		{
			LOG_INFO << "server error : no such file";
			conn->shutdown();
		}
		FilePtr filePtr(file,::fclose);
		char buf[writeSize_];
		int numBytes = fread(buf,1,sizeof buf,file);
		conn->send(buf,numBytes);
		conn->setContext(filePtr);
	}
}

void DownloadServer::writeCompleteCallback(const muduo::net::TcpConnectionPtr& conn)
{
	const FilePtr& filePtr = boost::any_cast<const FilePtr&>(conn->getContext());

	if(filePtr == NULL)
	{
		conn->shutdown();
		LOG_INFO << "error happens when get context";
	}
	char buf[writeSize_];
	int numBytes = fread(buf,1,sizeof buf,get_pointer(filePtr));
	if(numBytes > 0)
	{
		conn->send(buf,numBytes);
		conn->setContext(filePtr);
	}
	else
	{
		conn->shutdown();
		LOG_INFO << "shutdown here";
	}	
}

void DownloadServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
							   muduo::net::Buffer* buf,
							   muduo::Timestamp time)
{
	muduo::string msg(buf->retrieveAllAsString());
	LOG_INFO << "reveive " << msg.size() << " bytes from " 
	<< conn->peerAddress().toIpPort();
}
