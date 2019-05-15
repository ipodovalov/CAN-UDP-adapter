#ifndef FILELOGHANDLER_H
#define FILELOGHANDLER_H

#include "iloghandler.h"

#include <mutex>
#include <unordered_map>
#include <string>

#include <ctime>

extern "C" {
#include <sys/time.h>
}

#include <defines.h>
/*!
  Класс обработчика сообщений с записью в файл.

  \author Лебедь Дмитрий, lebed.dmitry@gmail.com
  */

class FileLogHandler : public ILogHandler
{
    int setInternalParameters(const std::unordered_map<std::string, std::string> & params);
public:
        FileLogHandler();
        virtual ~FileLogHandler();

        //! Установка параметров обработчика
        /*!
          \param params хэш-таблица с параметрами \n
          Описание возможных параметров: \n
          filename - имя файла \n
          truncate ("true"/"false") - урезать файл при открытии, или нет
          \return int >= 0 в случае успеха и < 0 в случае ошибки
          */
        virtual int setParameters(const std::unordered_map<std::string, std::string> & params);

        virtual int log(ELogger::InfoType type, ELogger::ErrorLevel level, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile, const std::string & str);

private:
        FILE *fp;
        std::mutex fileMutex;

        char timebuf[32];
        time_t t;
        struct tm * tmp;
        time_t nowtime;
        struct tm * nowtm;
        struct timeval timestamp;
        uint32_t log_limit_threshold;

        std::unordered_map<std::string, std::string> internalParams;

};

#endif // FILELOGHANDLER_H
