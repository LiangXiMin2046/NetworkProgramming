#include "/xmuduo/muduo/base/FileUtil.h"
#include "/xmuduo/muduo/base/Types.h"

#include <unistd.h>

using namespace muduo;

string getHostName()
{
	char buf[256];
	if(::gethostname(buf,sizeof buf) == 0)
	{
		buf[sizeof(buf)-1] = '\0';
		return buf;	
	}
	return "unknowhost";
}

string procname(const string& stat)
{
	if(stat.empty())
		return "";
	size_t lp = stat.find('(');
	size_t rp = stat.find(')');
	if(lp != string::npos && rp != string::npos && lp < rp)
		return stat.substr(lp+1,rp-lp-1);
	return "";
}

string proname()
{
	string result;
	FileUtil::readFile("/proc/self/status",65536,&result);
	return procname(result);
}
