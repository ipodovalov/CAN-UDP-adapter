#ifndef CONSOLELOGHANDLER_H
#define CONSOLELOGHANDLER_H

#include "iloghandler.h"

#include <mutex>
#include <string>

#include <ctime>
#include <cstdio>

extern "C" {
#include <sys/time.h>
} 

#include <defines.h>

/*!
  Класс обработчика сообщений с записью в консоль.

  \author Лебедь Дмитрий, lebed.dmitry@gmail.com
  */

class ConsoleLogHandler : public ILogHandler
{
public:
	ConsoleLogHandler();
	virtual ~ConsoleLogHandler();

	//! Установка параметров обработчика
	/*!
	  \param params хэш-таблица с параметрами \n
	  Описание возможных параметров: \n
	  output - если равен "stdout", то вывод будет производиться на стандартный поток,
	  в противном случае - в стандартный поток ошибок (stderr).
	  \return int >= 0 в случае успеха и < 0 в случае ошибки
	  */
    virtual int setParameters(const std::unordered_map<std::string, std::string> & params);

    virtual int log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str);

private:
    FILE *outFile;
    bool printLocation;
    std::mutex fileMutex;

	char timebuf[32];
	time_t nowtime;
	struct tm * nowtm;
	struct timeval timestamp;
};

#endif // CONSOLELOGHANDLER_H
