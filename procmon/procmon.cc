#include "plot.h"
#include "getname.h"

#include "/xmuduo/muduo/base/FileUtil.h"
#include "/xmuduo/muduo/base/ProcessInfo.h"
#include "/xmuduo/muduo/net/EventLoop.h"
#include "/xmuduo/muduo/net/http/HttpRequest.h"
#include "/xmuduo/muduo/net/http/HttpResponse.h"
#include "/xmuduo/muduo/net/http/HttpServer.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/type_traits/is_pod.hpp>

#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

//get info from /proc/pid/stat
struct StatData
{
	void parse(const char* startAtState,int kbPerPage)
	{
		std::istringstream iss(startAtState);
		iss >> state;
		iss >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags;
		iss >> minflt >> cminflt >> majflt >> cmajflt;
		iss >> utime >> stime >> cutime >> cstime;
		iss >> priority >> nice >> num_threads >> itrealvalue >> starttime;
		long vsize,rss;
		iss >> vsize >> rss >> rsslim;
		vsizeKb = vsize / 1024;
		rssKb = rss * kbPerPage;	
	}
	
	char state; //R:running,S:sleeping
	int ppid; //parent process id
	int pgrp; //parent process group id
	int session; //session group id
	int tty_nr; 
	int tpgid;
	int flags; //process flags

	long minflt;
	long cminflt;
	long majflt;
	long cmajflt;

	long utime; //time of task in user mode
	long stime; //time of task in kernel mode
	long cutime; //time of task wait for into user mode
	long cstime; //time of task wait fot into kernel mode

	long priority;
	long nice;
	long num_threads;
	long itrealvalue;
	long starttime; //time since task starts

	long vsizeKb; //vitual address space size
	long rssKb; //physical address space size,unit in page
	long rsslim; //max size of physical address space size;
};

BOOST_STATIC_ASSERT(boost::is_pod<StatData>::value);

class Procmon : boost::noncopyable
{
public:
	Procmon(EventLoop* loop,pid_t pid,uint16_t port,const char* proname)
		:	kClockTickPerSecond_(g_clockTicks),
 			kbPerPage_(g_pageSize / 1024),
            kBootTime_(getBootTime()),
            pid_(pid),
            server_(loop,InetAddress(port),getName()),
            procname_(procname(readProcFile("stat"))),  
            hostname_(getHostName()),
            cmdline_(getCmdLine()),
            ticks_(0),
            cpu_usage_(600 / kPeriod_),
            cpu_chart_(640,100,600,kPeriod_),
            ram_chart_(640,100,7200,30)
	{
		bzero(&lastStatData_,sizeof lastStatData_);
		server_.setHttpCallback(boost::bind(&Procmon::onRequest,this,_1,_2));
	}

	void start()
	{
		tick();
		server_.getLoop()->runEvery(kPeriod_,boost::bind(&Procmon::tick,this));
		server_.start();
	}

private:
	string getName() const
	{
		char name[256];
		snprintf(name,sizeof name,"procmon-%d",pid_);
		return name;
	}

	void onRequest(const HttpRequest& req,HttpResponse* resp)
	{
		resp->setStatusCode(HttpResponse::k200Ok);
		resp->setStatusMessage("OK");
		resp->setContentType("text/plain");
		resp->addHeader("Server","Muduo-Procmon");

		if(req.path() == "/")
		{
			resp->setContentType("text/html");
			fillOverview(req.query());
			resp->setBody(response_.retrieveAllAsString());
		}
		else if(req.path() == "/cmdline")
		{
			resp->setBody(cmdline_);
		}				
		else if(req.path() == "/cpu.png")
		{
			std::vector<double> cpu_usage;
			for(size_t i = 0; i < cpu_usage_.size(); ++i)
				cpu_usage.push_back(cpu_usage_[i].cpuUsage(kPeriod_,kClockTickPerSecond_));
			string png = cpu_chart_.plotCpu(cpu_usage);
			resp->setContentType("image/png");
			resp->setBody(png);
		}

		else if(req.path() == "/environ")
		{
			resp->setBody(getEnviron());
		}
		else if(req.path() == "/io")
		{
			resp->setBody(readProcFile("io"));
		}
		else if(req.path() == "/limits")
		{
			resp->setBody("limits");
		}
		else if(req.path() == "/smaps")
		{
			resp->setBody(readProcFile("smaps"));
		}
		else if(req.path() == "/status")
		{
			resp->setBody(readProcFile("status"));
		}
		else if(req.path() == "/threads")
		{
			fillThreads();
			resp->setBody(response_.retrieveAllAsString());
		}
		else
		{
			resp->setStatusCode(HttpResponse::k404NotFound);
			resp->setStatusMessage("Not Found");
			resp->setCloseConnection(true);
		}
	}

	void fillOverview(const string& query)
	{
		response_.retrieveAll();
		Timestamp now = Timestamp::now();
		appendResponse("<html><head><title>%s on %s</title>\n",
                       procname_.c_str(),hostname_.c_str());
		fillRefresh(query);
		appendResponse("</head><body>\n");
		string stat = readProcFile("stat");
		if(stat.empty())
		{
			appendResponse("<h1>PID %d doesn't exist.</h1></body></body>",pid_);
			return;
		}

		int pid = atoi(stat.c_str());
		assert(pid == pid_);
		muduo::string procname = proname();
		//std::cout << procname << std::endl;
		appendResponse("<h1>%s on %s</h1>\n",
                       procname.c_str(),hostname_.c_str());
		response_.append("<p>Refresh <a href=\"?refresh=1\">1s</a> ");
		response_.append("<a href=\"?refresh=2\">2s</a> ");
		response_.append("<a href=\"?refresh=5\">5s</a> ");
		response_.append("<a href=\"?refresh=15\">15s</a> ");
		response_.append("<a href=\"?refresh=60\">60s</a>\n");

		response_.append("<p><a href=\"/cmdline\">Command line</a>\n");
		response_.append("<a href=\"/environ\">Environment variables</a>\n");
		response_.append("<a href=\"/threads\">Threads</a>\n");	

		appendResponse("<p>Page generated at %s (UTC)",now.toFormattedString().c_str());
		response_.append("<p><table>");
		StatData statData;
		bzero(&statData,sizeof statData);
		
		muduo::string content = readProcFile("stat");
		size_t pos = content.find(')');
		content = content.substr(pos+1);
		statData.parse(content.c_str(),kbPerPage_);

		appendTableRow("PID",pid);
		Timestamp started(getStartTime(statData.starttime));
		appendTableRow("start at UTC:",started.toFormattedString().c_str());
		appendTableRowFloat("Uptime (s)",timeDifference(now,started));
		appendTableRow("Excutable",readLink("exe").c_str());
		appendTableRow("Current dir",readLink("cwd").c_str());

		appendTableRow("state",getState(statData.state));
		appendTableRowFloat("User time(s)",getSeconds(statData.utime));
		appendTableRowFloat("System time(s)",getSeconds(statData.stime));

		appendTableRow("VmSize (KiB)",statData.vsizeKb);
		appendTableRow("VmRss (KiB)",statData.rssKb);
		appendTableRow("Threads",statData.num_threads);
		appendTableRow("CPU usage","<img src= \"/cpu.png\" height=\"100\" witdh=\"640\">");
		
		appendTableRow("Minor page faults",statData.minflt);
		appendTableRow("Major page faults",statData.majflt);

		response_.append("</table>");
		response_.append("</body></html>");	
	}

	void fillRefresh(const string& query)
	{
		size_t p = query.find("refresh=");
		if(p != string::npos)
		{
			int seconds = atoi(query.c_str()+p+8);
			if(seconds > 0)
			{
				appendResponse("<meta http-equiv=\"refresh\" content=\"%d\">\n",seconds);
			}
		}
	}
	
	void fillThreads()
	{
		response_.retrieveAll();
	}

	string readProcFile(const char* basename)
	{
		char filename[256];
		snprintf(filename,sizeof filename,"/proc/%d/%s",pid_,basename);
		string content;
		FileUtil::readFile(filename,1024*1024,&content);
		return content;
	}

	string readLink(const char* basename)
	{
		char filename[256];
		snprintf(filename,sizeof filename,"/proc/%d/%s",pid_,basename);
		char link[1024];
		ssize_t len = ::readlink(filename,link,sizeof link);
		string result;
		if(len > 0)
		{
			result.assign(link,len);
		}
		return result;
	}
	
	//in class member function,string-index and first -to-check should add 1,fuck
	int appendResponse(const char* fmt, ...)__attribute__((format(printf,2,3)));
	
	void appendTableRow(const char* name,long value)
	{
		appendResponse("<tr><td>%s</td><td>%ld</td></tr>\n",name,value);
	}

	void appendTableRowFloat(const char* name,double value)
	{
		appendResponse("<tr><td>%s</td><td>%.2f</td></tr>\n",name,value);
	}

	void appendTableRow(const char* name,const char* value)
	{
		appendResponse("<tr><td>%s</td><td>%s</td></tr>\n",name,value);
	}
	
	string getCmdLine()
	{
		return boost::replace_all_copy(readProcFile("cmdline"),string(1,'\0'),"\n\t");
	}

	string getEnviron()
	{
		return boost::replace_all_copy(readProcFile("enviro"),string(1,'\0'),"\n");
	}

	Timestamp getStartTime(long starttime)
	{
		return Timestamp(Timestamp::kMicroSecondsPerSecond * kBootTime_
                         + Timestamp::kMicroSecondsPerSecond * starttime / kClockTickPerSecond_);
	}

	double getSeconds(long ticks)
	{
		return static_cast<double>(ticks) / kClockTickPerSecond_;
	}

	void tick()
	{
		string stat = readProcFile("stat");
		if(stat.empty())
			return;
		size_t pos = stat.find(')');
		assert(pos != std::string::npos);
		stat = stat.substr(pos+1);
		StatData statData;
		bzero(&statData,sizeof statData);
		statData.parse(stat.c_str(),kbPerPage_);
		if(ticks_ > 0)
		{
			CpuTime time;
			time.userTime_ = std::max(0,static_cast<int>(statData.utime - lastStatData_.utime));
			time.sysTime_ = std::max(0,static_cast<int>(statData.stime - lastStatData_.stime)); 
			cpu_usage_.push_back(time);
		}

		lastStatData_ = statData;
		++ticks_; 
	}

	//some static member function
	static const char* getState(char state)
	{
		switch(state)
		{
			case 'R':
				return "Running";
			case 'S':
				return "Sleeping";
			case 'D':
				return "Disk sleep";
			case 'Z':
				return "Zombie";
			default:
				return "Unknow";
		}
	}

	static long getLong(const string& status,const char* key)
	{
		long result = 0;
		size_t pos = status.find(key);
		if(pos != std::string::npos)
		{
			result = ::atol(status.c_str()+pos+strlen(key));
		}
		return result;
	}

	static long getBootTime()
	{
		string stat;
		FileUtil::readFile("/proc/stat",65536,&stat);
		return getLong(stat,"btime ");
	}

	struct CpuTime
	{
		int userTime_;
		int sysTime_;
		double cpuUsage(double kPeriod,double kClockTickPerSecond) const
		{
			return (userTime_ + sysTime_) / (kClockTickPerSecond * kPeriod);
		}
	};

	const static int kPeriod_ = 2.0;
	const int kClockTickPerSecond_;
	const int kbPerPage_;
	const long kBootTime_;
	const pid_t pid_;
	HttpServer server_;
	const string procname_;
	const string hostname_;
	const string cmdline_;
	int ticks_;
	StatData lastStatData_;
	boost::circular_buffer<CpuTime> cpu_usage_;
	Plot cpu_chart_;
	Plot ram_chart_;
	
	Buffer response_;
};

bool processExists(pid_t pid)
{
	char buf[256];
	snprintf(buf,sizeof buf,"/proc/%d/stat",pid);
	return ::access(buf,R_OK) == 0;
}

int Procmon::appendResponse(const char* fmt,...)
{
	char buf[1024];
	va_list args;
	va_start(args,fmt);
	int ret = vsnprintf(buf,sizeof buf,fmt,args);
	va_end(args);
	response_.append(buf);
	return ret;
}

int main(int argc,char* argv[])
{
	if(argc < 3)
	{
		printf("Usage:%s pid prot [name]\n",argv[0]);
		return 0;
	}

	int pid = atoi(argv[1]);
	if(!processExists(pid))
	{
		printf("Process %d doesn't exist. \n",pid);
		return 1;
	}

	EventLoop loop;
	uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
	Procmon procmon(&loop,pid,port,argc > 3 ? argv[3] : "");
	procmon.start();
	loop.loop();
	return 0;
}
