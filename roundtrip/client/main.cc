#include "client.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc,char* argv[])
{
	if(argc !=3)
	{
		printf("wrong num of arguements!\n");
		exit(1);
	}
	EventLoop loop;
	short port = static_cast<short>(atoi(argv[2]));
	InetAddress addr(argv[1],port);
	RTTClient client(&loop,addr);
	client.connect();
	loop.loop();
	return 0;
}
 
