#include "chargen.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"

using namespace muduo;
using namespace muduo::net;

int main(void)
{
	EventLoop loop;
	InetAddress addr(9527);
	ChargenServer server(&loop,addr);
	server.start();
	loop.loop();
	return 0;
}
