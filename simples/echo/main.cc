#include "echo.h"

#include "/xmuduo/muduo/net/EventLoop.h"

int main(void)
{
	muduo::net::EventLoop loop;
	muduo::net::InetAddress addr(9527);
	EchoServer server(&loop,addr);
	server.start();
	loop.loop();
	return 0;
}
