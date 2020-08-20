#include "client.h"

#include "/xmuduo/muduo/net/InetAddress.h"
#include "/xmuduo/muduo/net/EventLoopThread.h"
#include <string>
#include <stdio.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

int main(int argc,char* argv[])
{
	if(argc != 3)
	{
		printf("error arguements num\n");
		exit(1);
	}	
	short port = static_cast<short>(atoi(argv[2]));
	InetAddress addr(argv[1],port);
	EventLoopThread loopThread;
	ChatClient client(loopThread.startLoop(),addr);	
	client.connect();
	std::string msg;
	while(getline(std::cin,msg))
	{
		if(msg == "./quit")
		{
			client.disconnect();
			break;
		}
		else
		{
			client.write(msg);
		}		
	}
	return 0;
}	
