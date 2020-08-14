#include "server.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"
#include "/xmuduo/muduo/base/Types.h"

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int main(int argc,char* argv[])
{
	if(argc != 2)
	{
		printf("error args num!\n");
		exit(1);
	}
	EventLoop loop;
	InetAddress addr(9527);
	muduo::string fileLocation(argv[1]);
	DownloadServer server(&loop,addr,fileLocation);
	server.start();
	loop.loop();
	return 0;
}
