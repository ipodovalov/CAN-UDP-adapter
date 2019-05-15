#include "consoleloghandler.h"

#include <cstdio>
#include <iostream>

ConsoleLogHandler::ConsoleLogHandler() :
        outFile(stderr), printLocation(false)
{
}

ConsoleLogHandler::~ConsoleLogHandler()
{
}


int ConsoleLogHandler::setParameters(const std::unordered_map<std::string, std::string> & params) {
    if (params.find("output") != params.end() && params.at("output") == "stdout") {
        outFile = stdout;
	}

    if (params.find("print_location") != params.end() && params.at("print_location") == "true") {
        printLocation = true;
    }

	return 0;
}

int ConsoleLogHandler::log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str) {
    gettimeofday(&timestamp, NULL);
    nowtime = timestamp.tv_sec;
    nowtm = localtime(&nowtime);
	
	if (unlikely(nowtm == NULL)) {
		std::cerr << __FUNCTION__ << " Невозможно получить значение текущего времени!" << std::endl;
		return -1;
	}

	if (unlikely(strftime(timebuf, sizeof(timebuf), "%a %d.%m.%y %H:%M:%S", nowtm) == 0)) {
		std::cerr << __FUNCTION__ << " strftime returned 0" << std::endl;
		return -1;
	}


	fileMutex.lock();

    if (printLocation) {
        fprintf(outFile, "%s.%.3ld  [ %s - %s ] ( %s:%u - %s )  %s\n", timebuf, timestamp.tv_usec/1000,
                ELogger::infoTypeStr(type).c_str(), ELogger::errorLevelStr(level).c_str(),
                sourceFile.c_str(), sourceLine, sourceFunc.c_str(), str.c_str());
    } else {
        fprintf(outFile, "%s.%.3ld  [ %s - %s ]  %s\n", timebuf, timestamp.tv_usec/1000, ELogger::infoTypeStr(type).c_str(),
                ELogger::errorLevelStr(level).c_str(), str.c_str());
    }

	fileMutex.unlock();

	return 0;

}
