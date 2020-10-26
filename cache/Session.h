#ifndef CACHE_SERVER_SESSION_H
#define CACHE_SERVER_SESSION_H

#include "Item.h"

#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>

#include <functional>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/tokenizer.hpp>

using muduo::string;

class CacheServer;

class Session : boost::noncopyable,
                public boost::enable_shared_from_this<Session>
{
public:
	Session(CacheServer* owner,const muduo::net::TcpConnectionPtr& conn)
      :  owner_(owner),
         conn_(conn),
         state_(kNewCommand),
         protocol_(kAscii),
         noreply_(false),
         policy_(Item::kInvalid),
         bytesToDiscard_(0),
         needle_(Item::makeItem(kLongestKey,0,0,2,0)),
         bytesRead_(0),
         requestsProcessed_(0)
	{
		conn_->setMessageCallback(
             std::bind(&Session::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
	}

	~Session()
	{
		LOG_INFO << "requests processed: " << requestsProcessed_
                 << " input buffer size: " << conn_->inputBuffer()->internalCapacity()
                 << " output buffer size: " << conn_->outputBuffer()->internalCapacity()
                 << " Session output buffer size: " << outputBuf_.internalCapacity();
	}
private:
	enum State
	{
		kNewCommand,
		kReceiveValue,
		kDiscardValue,
	};

	enum Protocol
	{
		kAscii,
		kBinary,
		kAuto,
	};

	void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp);
	void onWriteComplete(const muduo::net::TcpConnectionPtr& conn);
	void receiveValue(muduo::net::Buffer* buf);
	void discardValue(muduo::net::Buffer* buf);

	//return true if finished a request
	bool processRequest(muduo::StringPiece request);
	void resetRequest();
	void reply(muduo::StringPiece msg);

	//word-seprator functional object
	struct SpaceSeparator
	{
		void reset() {}
		template <typename InputIterator,typename Token>
		bool operator() (InputIterator& next,InputIterator end,Token& tok);
	};

	typedef boost::tokenizer<SpaceSeparator,
         const char*,
         muduo::StringPiece> Tokenizer;

	struct Reader;
	bool doUpdate(Tokenizer::iterator& beg,Tokenizer::iterator end);
	bool doDelete(Tokenizer::iterator& beg,Tokenizer::iterator end);

	CacheServer* owner_;
	muduo::net::TcpConnectionPtr conn_;
	State state_;
	Protocol protocol_;

	//current request
	string command_;
	bool noreply_;
	Item::UpdatePolicy policy_;
	ItemPtr currItem_;
	size_t bytesToDiscard_;
	//cached
	ItemPtr needle_;
	muduo::net::Buffer outputBuf_;

	//per session stats
	size_t bytesRead_;
	size_t requestsProcessed_;

	static string kLongestKey;
	static const int kMaxSendLen = 64 * 1024;
};

typedef boost::shared_ptr<Session> SessionPtr;

#endif
