#ifndef NETWORKPROGRAMMING_CHATROOM_CODEC_H
#define NETWORKPROGRAMMING_CHATROOM_CODEC_H

#include "/xmuduo/muduo/net/Endian.h"
#include "/xmuduo/muduo/net/TcpConnection.h"
#include "/xmuduo/muduo/net/Buffer.h"
#include "/xmuduo/muduo/base/Logging.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class LengthHeaderCodec : boost::noncopyable
{
public:
	typedef boost::function<void(const muduo::net::TcpConnectionPtr& conn,
								 const muduo::string& message,
								 muduo::Timestamp)> StringMessageCallback;
	
	explicit LengthHeaderCodec(const StringMessageCallback& cb)
		:	messageCallback_(cb)
	{
	}

	void send(muduo::net::TcpConnection* conn,
			  const muduo::StringPiece& msg)
	{
		muduo::net::Buffer outputBuffer;
		outputBuffer.append(msg.data(),msg.size());
		int32_t msgSize = static_cast<int32_t>(msg.size());
		int32_t be32 = muduo::net::sockets::hostToNetwork32(msgSize);
		outputBuffer.prepend(&be32,sizeof(int32_t));
		conn->send(&outputBuffer);
	}

	void onMessage(const muduo::net::TcpConnectionPtr& conn,
				   muduo::net::Buffer* buf,
				   muduo::Timestamp time)
	{
		while(buf->readableBytes() >= kHeaderLen) //using while to process multi msgs
		{
			int32_t len = buf->peekInt32();
			if(len < 0 || len > 65536)
			{
				LOG_ERROR << "length of msg is error" << len;
				conn->shutdown();
				break;
			}
			else if(buf->readableBytes() >= kHeaderLen + len)
			{
				buf->retrieve(kHeaderLen);
				muduo::string msg(buf->peek(),len);
				buf->retrieve(len);
				messageCallback_(conn,msg,time);
			}
			else
			{
				break;
			}
		}	
	}

private:
	StringMessageCallback messageCallback_;
	const static size_t kHeaderLen = sizeof(int32_t);
};

#endif
