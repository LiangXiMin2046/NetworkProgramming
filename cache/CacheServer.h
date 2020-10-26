#ifndef CACHED_SERVER_CACHEDSERVER_H
#define CACHED_SERVER_CACHEDSERVER_H

#include "Item.h"
#include "Session.h"

#include <muduo/base/Mutex.h>
#include <muduo/net/TcpServer.h>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/scoped_ptr.hpp>

class CacheServer : boost::noncopyable
{
public:
	struct Options
	{
		Options();
		uint16_t tcpport;
		uint16_t udpport;
		uint16_t gperfport;
		int threads;
	};

	CacheServer(muduo::net::EventLoop* loop,const Options&);
	~CacheServer();

	void setThreadNum(int threads)
	{
		server_.setThreadNum(threads);
	}
	
	void start();
	void stop();
	time_t startTime() const
	{
		return startTime_;
	}

	bool storeItem(const ItemPtr& item,Item::UpdatePolicy policy,bool* exists);
	ConstItemPtr getItem(const ConstItemPtr& key) const;
	bool deleteItem(const ConstItemPtr& key);

private:
	void onConnection(const muduo::net::TcpConnectionPtr& conn);

	struct Stats;

	muduo::net::EventLoop* loop_;
	Options options_;
	const time_t startTime_;

	mutable muduo::MutexLock mutex_;
	boost::unordered_map<string,SessionPtr> sessions_;
	struct Hash
	{
		size_t operator() (const ConstItemPtr& x) const
		{
			return x->hash();
		}
	};

	struct Equal
	{
		bool operator() (const ConstItemPtr& x,const ConstItemPtr& y) const
		{
			return x->key() == y->key();
		}
	};

	typedef boost::unordered_set<ConstItemPtr,Hash,Equal> ItemMap;

	struct MapWithLock
	{
		ItemMap items;
		mutable muduo::MutexLock mutex;
	};

	const static int kShards = 4096;

	boost::array<MapWithLock,kShards> shards_;

	muduo::net::TcpServer server_;
	boost::scoped_ptr<Stats> stats_;
};

#endif
