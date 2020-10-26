#include "SudokuServer.h"

#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace muduo;
using namespace muduo::net;
using namespace std;

bool parseCommand(int argc,char* argv[],uint16_t* port,int* threadNum)
{
	*port = 9527;
	*threadNum = 1;
	po::options_description desc("Allowed options");
	desc.add_options()
         ("help,h","Help")
         ("port,p",po::value<uint16_t>(port),"Port")
         ("threadnum,t",po::value<int>(threadNum),"Thread Num")
         ;
	po::variables_map vm;
	po::store(po::parse_command_line(argc,argv,desc),vm);
	po::notify(vm);

	if(vm.count("help"))
	{
		cout << desc << endl;
		return false;
	}
	return true;
}

int main(int argc,char* argv[])
{
	uint16_t port;
	int threadNum;
	if(parseCommand(argc,argv,&port,&threadNum))
	{
		EventLoop loop;
		InetAddress addr(port);
		SudokuServer server(&loop,addr,threadNum);
		server.start();
		loop.loop();				
	}
	return 0;
}
