#ifndef ELOGGER_H
#define ELOGGER_H

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <cstdint>


class ILogHandler;

/*!
  Класс для ведения логгирования.
  Для упрощения работы класс использует в основном статические методы.
  Вариация на тему singleton.\n
  Класс старается быть потокобезопасным, кроме функций по установке обработчиков (setLogger, appendLogger, resetLogger).
  Данные функции должны быть вызваны в начале программы.
  Для обеспечения полной потокобезопасности необходимо определить символ ELOGGER_FULL_THREAD_SAFE\n
  Поддерживаются типы информации в сообщениях (финансовая, системная, о работе устройств, и т.п.),
  а так же уровни важности (от trace до fatal).
  Для связки тип информации - диапазон уровней важности можно создать набор обработчиков.
  Легко расширяемые возможности по добавлению новых хранилищ логов при помощи наследования от класса ILogHandler.\n
  Перед началом использования класса необходимо вызвать функцию checkInit().\n
  Пример использования:\n
  ELogger(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Устройство не отвечает. Ошибка:" << errorStr;
  \sa ILogHandler checkInit
  \todo Добавить грамотное освобожедение ресурсов при завершении работы программы

  \author Лебедь Дмитрий, lebed.dmitry@gmail.com
  */

#define ELOG(TYPE, LEVEL) ELogger(TYPE, LEVEL, __LINE__, __PRETTY_FUNCTION__, __FILE__)

class ELogger
{
public:
	//! Тип информации в сообщении
    enum InfoType : uint8_t {
		//! Системная информация общего вида
        INFO_SYSTEM		= 1 << 0,
		//! Информация о работе устройств
        INFO_DEVICE		= 1 << 1,
        //! Информация об обмене данными
        INFO_COMM       = 1 << 2,
		//! Другая информация
        INFO_OTHER		= 1 << 7
    };

	//! Уровень важности сообщения
    enum ErrorLevel : uint8_t {
		//! Сообщение, отражающее ход выполнения программы
        LEVEL_TRACE = 1 << 0,
		//! Сообщение с отладочной информацией
        LEVEL_DEBUG = 1 << 1,
		//! Информационное сообщение, сигнализирующее об успешном событии
        LEVEL_INFO	= 1 << 2,
		//! Предупреждение о возможно неверном ходе выполнения программы
        LEVEL_WARN	= 1 << 5,
		//! Сообщение об ошибке
        LEVEL_ERROR	= 1 << 6,
		//! Сообщение о серьёзной ошибке
        LEVEL_FATAL = 1 << 7
    };

	//! Тип для задания диапазона важности сообщений
	typedef struct ELevelRange {
		//! Минимальное значение диапазона
		ErrorLevel min;
		//! Максимальное значение диапазона
		ErrorLevel max;
	} ELevelRange;

	//! Тип для задания обработчика для диапазона важностей сообщений
	typedef struct ELevelRangeHandler {
		//! Структура со значениями диапазона
		ELevelRange range;
		//! Указатель на класс-обработчик сообщений
		ILogHandler * handler;
	} ELevelRangeHandler;

	//! Конструктор класса
	/*!
	  \param infoType тип информации в сообщении \sa InfoType
	  \param errorLevel уровень важности сообщения \sa ErrorLevel
	  */
    ELogger(InfoType infoType, ErrorLevel errorLevel);

    //! Конструктор класса
    /*!
      \param infoType тип информации в сообщении \sa InfoType
      \param errorLevel уровень важности сообщения \sa ErrorLevel
      */
    ELogger(InfoType infoType, ErrorLevel errorLevel, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile);

	//! Деструктор класса
	/*!
	  При вызове деструктора класса данные из строки-буфера передаются обработчикам логгирования
	  для последующей записи по местам назначения (файлам, БД, и т.п.).
	  */
    ~ELogger();

	//! Установка обработчика для типа информации и диапазона важностей
	/*!
	  Устанавливается обработчик для данного типа информации.
	  При этом все остальные обработчики для данного типа информации сбрасываются.
	  \param type тип информации сообщения
	  \param handler обработчик сообщений
	  \param min минимальный уровень важности для сообщения
	  \param max максимальный уровень важности для сообщения
	  \sa InfoType ErrorLevel ILogHandler appendLogger
	  */
	static void setLogger(InfoType type, ILogHandler * handler, ErrorLevel min = LEVEL_TRACE, ErrorLevel max = LEVEL_FATAL);

	//! Добавление обработчика для типа информации и диапазона важностей
	/*!
	  В конец списка обработчиков добавляется обработчик для данного типа информации.
	  При этом все остальные обработчики для данного типа информации остаются и обрабатываются по очереди.
	  \param type тип информации сообщения
	  \param handler обработчик сообщений
	  \param min минимальный уровень важности для сообщения
	  \param max максимальный уровень важности для сообщения
	  \sa InfoType ErrorLevel ILogHandler setLogger
	  */
	static void appendLogger(InfoType type, ILogHandler * handler, ErrorLevel min = LEVEL_TRACE, ErrorLevel max = LEVEL_FATAL);

	//! Сбросить все сторонние обработчики для заданного типа информации и установить обработчик по умолчанию
	/*!
	  Сбрасываются все назначенные обработчики  для данного типа информации.
	  Устанавливается обработчик по умолчанию: вывод на консоль.
	  \param type тип информации
	  */
	static void resetLogger(InfoType type);

	//! Сбросить все сторонние обработчики для заданного типа информации
	/*!
	  Сбрасываются все назначенные обработчики  для данного типа информации.
	  Обработчик по умолчанию не устанавливается.
	  \param type тип информации
	  */
	static void clearLogger(InfoType type);

	//! Инициализпция логгера
	/*!
	  Необходимо выполнить один раз перед использованием других фукнций логгера
	  при старте программы из одного из потоков.
	  Инициализирует внутренние структуры данных.
	  */
	static void initLogger();

	//! Очистить все данные, специфичные для данного потока
	/*!
	  На данный момент функционал не реализован
	  \todo Реализовать освобождение служебных структур данных при завершении потока
	  */
	static void threadExiting();

	// Далее идут операторы для записи данных

	//! Записывает строку в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(const std::string & t) { stream << '\'' << t << '\'' << " ";  return *this; }
	//! Записывает строку в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(const char* t) { stream << t << " "; return *this; }
	//! Записывает булев тип в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(bool t) { stream << t << " "; return *this; }
	//! Записывает символ в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(char t) { stream << t << " "; return *this; }
    //! Записывает число в HEX в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(unsigned char t) { stream << std::hex << (int)t << std::dec << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(signed short t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(unsigned short t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(signed int t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(unsigned int t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(signed long t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(unsigned long t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(long long int t) { stream << t << " "; return *this; }
	//! Записывает число в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(unsigned long long int t) { stream << t << " "; return *this; }
	//! Записывает число с плавающей точкой в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(float t) { stream << t << " "; return *this; }
	//! Записывает число с плавающей точкой в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(double t) { stream << t << " "; return *this; }
	//! Записывает указатель в буфер, добавляет к ее концу пробел, возвращает ссылку на класс логгера
    inline ELogger &operator<<(const void * t) { stream << t << " "; return *this; }

    inline ELogger &operator<<(const std::_Setfill<char> &fill) { stream << fill; return *this; }
    inline ELogger &operator<<(const std::_Setbase &base) { stream << base; return *this; }
    inline ELogger &operator<<(const std::_Setw &w) { stream << w; return *this; }

    inline ELogger &operator<<(std::ios_base& (*m)(std::ios_base &)) {  m(stream); return *this; }

    inline ELogger &operator<<(const std::basic_string<uint8_t> &buf) {
        std::ios  state(NULL);

        state.copyfmt(stream);

        stream << "[ ";

        stream << std::hex << std::uppercase << std::setfill('0') << std::setw(2);

        for (size_t i = 0; i < buf.size(); i++) {
            stream << (int)buf[i] << " ";
        }

        stream.copyfmt(state);

        stream << "] ";

        return *this;
    }


	//! Возвращает строковое описание типа сообщения
	/*!
	  Например, infoTypeStr(INFO_SYSTEM) = "system"
	  \param type тип сообщения
	  \return строка, описание типа сообщения
	  */
    static std::string infoTypeStr(InfoType type);

	//! Возвращает строковое описание уровня важности сообщения
	/*!
	  Например, errorLevelStr(LEVEL_TRACE) = "trace"
	  \param level уровень важности сообщения
	  \return строка, описание уровня важности сообщения
	  */
    static std::string errorLevelStr(ErrorLevel level);

	//! Возвращает тип сообщения, соответствующий описывающей его строке
	/*!
	  \param str строковое описание типа
	  \return тип сообщения, InfoType. При несовпадении ни с одним типом возвращается 0.
	  */
    static uint32_t typeForStr(const std::string &str);

	//! Возвращает уровень важности сообщения, соответствующий описывающей его строке
	/*!
	  \param str строковое описание уровня важности
	  \return важность сообщения, ErrorLevel. При несовпадении ни с одним типом возвращается 0.
	  */
    static uint32_t errorLevelForStr(const std::string &str);

protected:
	//! Проверка, содержится ли значение уровня важности сообщения в диапазон
	/*!
	  \param level уровень важности сообщения
	  \param range диапазон уровней важности
	  \return true, если данное значение содержится в диапазоне, false в противном случае
	  */
	static inline bool checkELevelInterval(ErrorLevel level, const ELevelRange & range) {
		return (range.max >= level && range.min <= level) ? true : false;
	}

	//! Сформировать структуру для описания обработчика диапазона важностей сообщения
	/*!
	  \param handler указатель на класс обработчика сообщений
	  \param min минимальный уровень важности
	  \param max максимальный уровень важности
	  \return структуру ELevelRangeHandler, с заданным обработчиком и интервалом важностей
	  \sa ILogHandler
	  */
	static inline ELevelRangeHandler createRangeHandler(ILogHandler * handler, ErrorLevel min, ErrorLevel max);

private:
    const InfoType infoType;
    const ErrorLevel errorLevel;

    static std::mutex handlersMutex;
	static bool initialized;
	// Хэш списков обработчиков логгирования
    static std::unordered_map<uint8_t, std::list<ELevelRangeHandler> > handlersHash;

    // Текстовый поток
    std::ostringstream stream;

    const uint32_t sourceLine;
    const std::string sourceFunc;
    const std::string sourceFile;

	// Инициализированы ли строковые хэши
	static bool strHashesInitialized;
	// Сами строковые хэши
    static std::unordered_map<uint32_t, std::string> msgTypeStrHash, errorLevelStrHash;
    static std::unordered_map<std::string, uint32_t> strMsgTypeHash, strErrorLevelHash;

};

ELogger::ELevelRangeHandler ELogger::createRangeHandler(ILogHandler * handler, ELogger::ErrorLevel min, ELogger::ErrorLevel max) {
		ELevelRange range;
		ELevelRangeHandler rangeHandler;
		range.max = max;
		range.min = min;
		rangeHandler.handler = handler;
		rangeHandler.range = range;
		return rangeHandler;
}

#endif // ELOGGER_H
