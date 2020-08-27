#ifndef NETPROGRAMMING_TIMEWHEEL_SERVER_H
#define NETPROGRAMMING_TIMEWHEEL_SERVER_H

#include "/xmuduo/muduo/base/copyable.h"
#include "/xmuduo/muduo/net/TcpServer.h"

#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_set.hpp>
#include <boost/circular_buffer.hpp>

class EchoServer : public boost::noncopyable
{
public:
	EchoServer(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& addr,
               int listSize);

	void start();

private:
	typedef boost::weak_ptr<muduo::net::TcpConnection> WeakTcpConnectionPtr;

	struct Entry : public muduo::copyable
	{
		explicit Entry(const WeakTcpConnectionPtr& weakConn)
          :  weakConn_(weakConn)
		{
		}
		
		~Entry()
		{
			muduo::net::TcpConnectionPtr conn = weakConn_.lock();
			if(conn)
			{
				conn->shutdown();
			}
		}
		WeakTcpConnectionPtr weakConn_;								
	};

	typedef boost::shared_ptr<Entry> EntryPtr;
	typedef boost::weak_ptr<Entry> WeakEntryPtr;
	typedef boost::unordered_set<EntryPtr> Bucket;
	typedef boost::circular_buffer<Bucket> WeakConnectionList;

	void onTimer();

	void onConnection(const muduo::net::TcpConnectionPtr& conn);

	void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);

	muduo::net::EventLoop* loop_;
	muduo::net::TcpServer server_;
	WeakConnectionList connectionBuckets_;
};

#endif
