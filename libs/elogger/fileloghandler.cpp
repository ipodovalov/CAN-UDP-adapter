#include "fileloghandler.h"

#include <cstdio>
#include <iostream>




FileLogHandler::FileLogHandler() :
        fp(stderr), log_limit_threshold(1)
{

}

FileLogHandler::~FileLogHandler()
{
    if (fp != stderr && fp != NULL)
        fclose(fp);
}

int FileLogHandler::setInternalParameters(const std::unordered_map<std::string, std::string> & params)
{
    if (params.find("filename") != params.end()) {

        if(fp != stderr && fp != NULL)
            fclose(fp);

            if (params.at("truncate") == "true")
                fp = fopen(params.at("filename").c_str(), "w+");
            else
                fp = fopen(params.at("filename").c_str(), "a+");

            if (unlikely(fp == NULL)) {
                std::cerr << __FUNCTION__ << " Невозможно открыть файл для добавления записей!" << std::endl;
                return -1;
            }

    }

    return 1;
}

int FileLogHandler::setParameters(const std::unordered_map<std::string, std::string> & params) {
    auto i = params.begin();
    while (i != params.end()) {
        this->internalParams[i->first] = i->second;
        ++i;
    }
    return this->setInternalParameters(internalParams);
}


int FileLogHandler::log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str) {
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

    if (likely(fp != NULL))
    {
        fileMutex.lock();

        fprintf(fp, "%s.%.3ld  [ %s - %s ]  %s\n", timebuf, timestamp.tv_usec/1000, ELogger::infoTypeStr(type).c_str(), ELogger::errorLevelStr(level).c_str(), str.c_str());
        fflush(fp);
        fileMutex.unlock();

        return 0;
    } else {
        std::cerr << "[E] " << __FUNCTION__ << " Не могу записать в файл лога! Файловый дескриптор == NULL" << std::endl;
        return -1;
    }
}
