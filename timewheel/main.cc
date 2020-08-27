//receive 2 arguements
//argv[1] : size of time wheel

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
		printf("wrong num of arguements!\n");
		exit(1);
	}

	int size = atoi(argv[1]);
	EventLoop loop;
	InetAddress addr(9527);
	EchoServer server(&loop,addr,size);
	server.start();
	loop.loop();	
	return 0;
}
