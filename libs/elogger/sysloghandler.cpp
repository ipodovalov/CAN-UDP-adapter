#include "sysloghandler.h"


bool SysLogHandler::initialized = false;

SysLogHandler::SysLogHandler()
{
}

SysLogHandler::~SysLogHandler()
{
	if (initialized)
		closelog();
}

int SysLogHandler::setParameters(const std::unordered_map<std::string, std::string> & params) {
    std::lock_guard<std::mutex> lock(initMutex);

	if (initialized)
		closelog();

	int option = 0;

    if (params.at("withpid") == "true")
		option |= LOG_PID;

    if (params.find("ident") != params.end() && !params.at("ident").empty())
        openlog(params.at("ident").c_str(), option, LOG_USER);
    else
        openlog("ELogger", option, LOG_USER);

	initialized = true;

	return 0;
}

int SysLogHandler::log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str) {
	initMutex.lock();

	if (unlikely(!initialized)) {
        openlog("ELogger", 0x00, LOG_USER);
		initialized = true;
	}

	initMutex.unlock();

    syslog(getLevel(level), "[ %s ] %s\n", ELogger::infoTypeStr(type).c_str(), str.c_str());

	return 0;
}
