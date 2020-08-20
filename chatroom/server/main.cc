//need 2 arguements
//argv[1] : port

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
		printf("need 2 arguements\n");
		exit(1);
	}
	EventLoop loop;
	int16_t port = static_cast<int16_t>(atoi(argv[1]));
	InetAddress addr(port);
	ChatServer server(&loop,addr);
	server.start();
	loop.loop();
	return 0;
}
