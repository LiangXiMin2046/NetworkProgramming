#ifndef NETPROGRAMMING_TTCP_COMMON_H
#define NETPROGRAMMING_TTCP_COMMON_H

#include <string>
#include <stdint.h>

struct Options
{
	uint16_t port;
	int length; //length of message
	int number; //number of message
	bool transmit,receive,nodelay;
	std::string host;
	Options()
		:	port(0),length(0),number(0),
            transmit(false),receive(false),nodelay(false)
	{
	}
};

bool parseCommandLine(int argc,char* argv[],Options* opt);
struct sockaddr_in resolveOrDie(const char* host,uint16_t port);

struct SessionMessage
{
	int32_t number;
	int32_t length;
}__attribute__((__packed__)); //gcc para

struct PayloadMessage
{
	int32_t length;
	char data[0]; //variable length
};

void transmit(const Options& opt);

void receive(const Options& opt);

#endif
