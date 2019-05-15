#ifndef SYSLOGHANDLER_H
#define SYSLOGHANDLER_H

#include "iloghandler.h"

#include <mutex>

#include <syslog.h>

#include <defines.h>

/*!
  Класс обработчика сообщений при помощи демона syslog.

  \author Лебедь Дмитрий, ОАО СКБ ВТ "Искра", dimaz.sertolovo@gmail.com
  */

class SysLogHandler : public ILogHandler
{
public:
	SysLogHandler();
	virtual ~SysLogHandler();

	//! Установка параметров обработчика
	/*!
	  \param params хэш-таблица с параметрами \n
	  Описание возможных параметров: \n
	  withpid = true - логгировать PID \n
	  ident - установить идентификатор процесса
	  \return int >= 0 в случае успеха и < 0 в случае ошибки
	  */
    virtual int setParameters(const std::unordered_map<std::string, std::string> & params);

    virtual int log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str);

protected:

	//! Перевод уровня важности с внутрибиблиотечного в тип syslog.h
	/*!
	  \param level уровень важности
	  \return уровень важности в типе syslog.h
	  */
    inline static int getLevel(ELogger::ErrorLevel level);


private:
    std::mutex initMutex;

	static bool initialized;

};

int SysLogHandler::getLevel(ELogger::ErrorLevel level) {
	switch (level) {
    case ELogger::LEVEL_TRACE:
		return LOG_NOTICE;
    case ELogger::LEVEL_DEBUG:
		return LOG_DEBUG;
    case ELogger::LEVEL_INFO:
		return LOG_INFO;
    case ELogger::LEVEL_WARN:
		return LOG_WARNING;
    case ELogger::LEVEL_ERROR:
		return LOG_ERR;
    case ELogger::LEVEL_FATAL:
		return LOG_CRIT;
	default:
		return LOG_INFO;
	}
}

#endif // SYSLOGHANDLER_H
