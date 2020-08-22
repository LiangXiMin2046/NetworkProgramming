#include "server.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc,char* argv[])
{
	if(argc != 2)
	{
		printf("wrong num of arguements");
		exit(0);
	}
	EventLoop loop;
	short port = static_cast<int>(atoi(argv[1]));
	InetAddress addr(port);
	RTTServer server(&loop,addr);
	server.start();
	loop.loop();
	return 0;
}
