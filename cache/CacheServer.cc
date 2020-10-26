#include "CacheServer.h"

#include <iostream>

#include <muduo/base/Atomic.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include <functional>

using namespace muduo;
using namespace muduo::net;

muduo::AtomicInt64 g_cas;

CacheServer::Options::Options()
{
	bzero(this,sizeof(*this));
}

struct CacheServer::Stats
{
};

CacheServer::CacheServer(muduo::net::EventLoop* loop,const Options& options)
	:	loop_(loop),
		options_(options),
		startTime_(::time(NULL)-1),
		server_(loop,InetAddress(options.tcpport),"cached-server"),
        stats_(new Stats)
{
	server_.setConnectionCallback(
        std::bind(&CacheServer::onConnection,this,std::placeholders::_1));
}

CacheServer::~CacheServer()
{
}

void CacheServer::start()
{
	server_.start();
}

void CacheServer::stop()
{
	loop_->runAfter(3.0,boost::bind(&EventLoop::quit,loop_));
}

bool CacheServer::storeItem(const ItemPtr& item,Item::UpdatePolicy policy,bool* exists)
{
	assert(item->neededBytes() == 0);
	muduo::MutexLock& mutex = shards_[item->hash() % kShards].mutex;
	ItemMap& items = shards_[item->hash() % kShards].items;
	muduo::MutexLockGuard lock(mutex);
	ItemMap::const_iterator it = items.find(item);
	*exists = it != items.end();
	if(policy == Item::kSet)
	{
		item->setCas(g_cas.incrementAndGet());
		if(*exists)
			items.erase(it);
		items.insert(item);
	}
	else
	{
		if(policy == Item::kAdd)
		{
			if(*exists)
			{
				return false;
			}
			else
			{
				item->setCas(g_cas.incrementAndGet());
				items.insert(item);
			}
		}
		else if(policy == Item::kReplace)
		{
			if(*exists)
			{
				item->setCas(g_cas.incrementAndGet());
				items.erase(it);
				items.insert(item);
			}
			else 
				return false;
		}
		else if(policy == Item::kAppend || policy == Item::kPrepend)
		{
			if(*exists)
			{
				const ConstItemPtr& oldItem = *it;
				int valueLen = static_cast<int>(item->valueLength()) + 
                                static_cast<int>(oldItem->valueLength());
				ItemPtr newItem(new Item(oldItem->key(),
                                         oldItem->flags(),
                                         oldItem->rel_exptime(),
                                         valueLen,
                                         g_cas.incrementAndGet()));
				if(policy == Item::kAppend)
				{
					newItem->append(oldItem->value(),oldItem->valueLength()-2);
					newItem->append(item->value(),item->valueLength());
				}
				else
				{
					newItem->append(item->value(),item->valueLength()-2);
					newItem->append(oldItem->value(),oldItem->valueLength());	
				}
				assert(newItem->neededBytes() == 0);
				assert(newItem->endWithCRLF());
				items.erase(it);
				items.insert(newItem);					
			}
			else
				return false;
		}
		else if(policy == Item::kCas)
		{
			if(*exists && (*it)->cas() == item->cas())
			{
				item->setCas(g_cas.incrementAndGet());
				items.erase(it);
				items.insert(item);
			}
			else
			{
				return false;
			}
		}
		else 
		{
			assert(false);
		}		
	}
	return true;
}

ConstItemPtr CacheServer::getItem(const ConstItemPtr& key) const
{
	muduo::MutexLock& mutex = shards_[key->hash() % kShards].mutex;
	const ItemMap& items = shards_[key->hash() % kShards].items;
	muduo::MutexLockGuard lock(mutex);	
	ItemMap::const_iterator it = items.find(key);
	return it == items.end() ?  ConstItemPtr() : *it; 
}

bool CacheServer::deleteItem(const ConstItemPtr& key)
{
	muduo::MutexLock& mutex = shards_[key->hash() % kShards].mutex;
	ItemMap& items = shards_[key->hash() % kShards].items;
	muduo::MutexLockGuard lock(mutex);	
	return items.erase(key) == 1;	
}

void CacheServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		SessionPtr session(new Session(this,conn));
		MutexLockGuard lock(mutex_);
		assert(sessions_.find(conn->name()) == sessions_.end());
		sessions_[conn->name()] = session;
	}
	else
	{
		MutexLockGuard lock(mutex_);
		assert(sessions_.find(conn->name()) != sessions_.end());
		sessions_.erase(conn->name());
	}
}
