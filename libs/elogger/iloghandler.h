#ifndef ILOGHANDLER_H
#define ILOGHANDLER_H

#include <string>
#include <unordered_map>

#include <elogger.h>

/*!
  Интерфейс обработчика сообщений.
  Позволяет установить параметры (к примеру параметры подключения к БД) и произвести запись сообщения.

  \author Лебедь Дмитрий, lebed.dmitry@gmail.com

  */

class ILogHandler
{
public:
	//! Пустой виртуальный деструктор
	virtual ~ILogHandler() {};

	//! Установка параметров обработчика
	/*!
	  \param params хэш-таблица с параметрами
	  \return int >= 0 в случае успеха и < 0 в случае ошибки
	  */
    virtual int setParameters(const std::unordered_map<std::string, std::string> & params) = 0;

	//! Произвести запись сообщения
	/*!
	  \param type тип информации для данной записи (финансовая, о работе устройств, и т.п.)
	  \param level уровень важности сообщения
	  \param str текст сообщения для записи
	  \return int >= 0 в случае успеха и < 0 в случае ошибки
	  */
    virtual int log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str) = 0;
};

#endif // ILOGHANDLER_H
