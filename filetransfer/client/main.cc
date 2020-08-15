//receive 3 arguements
//argv[1] : ip address
//argv[2] : port

#include "client.h"

#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/InetAddress.h"

#include <stdio.h>
#include <unistd.h>
#include <string>

using namespace muduo;
using namespace muduo::net;

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		printf("error number of arguements!\n");
		exit(1);		
	}

	EventLoop loop;	
	std::string ipAddr(argv[1]);
	short port = static_cast<short>(atoi(argv[2]));
	InetAddress serverAddr(ipAddr.c_str(),port);
	ReceiveClient client(&loop,serverAddr);
	client.connect();
	loop.loop();
	return 0;
}
