#include "elogger.h"
#include "iloghandler.h"
#include "consoleloghandler.h"


// Инициализация статических полей класса
std::mutex ELogger::handlersMutex;

bool ELogger::initialized = false;

std::unordered_map<uint8_t, std::list<ELogger::ELevelRangeHandler> > ELogger::handlersHash;

bool ELogger::strHashesInitialized = false;

std::unordered_map<uint32_t, std::string> ELogger::msgTypeStrHash, ELogger::errorLevelStrHash;

std::unordered_map<std::string, uint32_t> ELogger::strMsgTypeHash, ELogger::strErrorLevelHash;

// Конец инициализации статических полей класса

ELogger::ELogger(InfoType infoType, ErrorLevel errorLevel) :
        infoType(infoType), errorLevel(errorLevel), sourceLine(0)
{
    initLogger();
}

ELogger::ELogger(InfoType infoType, ErrorLevel errorLevel, uint32_t sourceLine, const std::string &sourceFunc, const std::string &sourceFile) :
    infoType(infoType), errorLevel(errorLevel), sourceLine(sourceLine), sourceFunc(sourceFunc), sourceFile(sourceFile)
{
    initLogger();
}

ELogger::~ELogger()
{
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    for (auto logLevelHandlerIterator = handlersHash[infoType].begin(); logLevelHandlerIterator != handlersHash[infoType].end(); ++logLevelHandlerIterator) {
        if (checkELevelInterval(errorLevel, (*logLevelHandlerIterator).range)) {
            (*logLevelHandlerIterator).handler->log(infoType, errorLevel, sourceLine, sourceFunc, sourceFile, stream.str());
        }
    }
}

void ELogger::setLogger(InfoType type, ILogHandler * handler, ErrorLevel min, ErrorLevel max) {
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    if (likely(handler != NULL)) {
        std::list<ELevelRangeHandler> handlersList;
        handlersList.push_back(createRangeHandler(handler, min, max));
        handlersHash[type] = handlersList;
    }
}

void ELogger::appendLogger(InfoType type, ILogHandler * handler, ErrorLevel min, ErrorLevel max) {
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    if (likely(handler != NULL)) {
        handlersHash[type].push_back(createRangeHandler(handler, min, max));
    }
}

void ELogger::resetLogger(InfoType type) {
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    handlersHash[type].clear();
    handlersHash[type].push_back(createRangeHandler(new ConsoleLogHandler(), LEVEL_TRACE, LEVEL_FATAL));
}

void ELogger::clearLogger(InfoType type) {
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    handlersHash[type].clear();;
}

void ELogger::initLogger() {
#ifdef ELOGGER_FULL_THREAD_SAFE
    std::lock_guard<std::mutex> lock(handlersMutex);
#endif

    if (unlikely(!initialized)) {
        std::list<ELevelRangeHandler> stdHandlerList;
        stdHandlerList.push_back(createRangeHandler(new ConsoleLogHandler(), LEVEL_TRACE, LEVEL_FATAL));

        handlersHash[INFO_SYSTEM] = stdHandlerList;
        handlersHash[INFO_DEVICE] = stdHandlerList;
        handlersHash[INFO_COMM] = stdHandlerList;
        handlersHash[INFO_OTHER] = stdHandlerList;

        initialized = true;
    }

    if (unlikely(!strHashesInitialized)) {
        // Инициализация для типа InfoTypesourceLine
        msgTypeStrHash[INFO_SYSTEM] = "system";
        strMsgTypeHash[msgTypeStrHash[INFO_SYSTEM]] = INFO_SYSTEM;
        msgTypeStrHash[INFO_DEVICE] = "device";
        strMsgTypeHash[msgTypeStrHash[INFO_DEVICE]] = INFO_DEVICE;
        msgTypeStrHash[INFO_COMM] = "comm";
        strMsgTypeHash[msgTypeStrHash[INFO_COMM]] = INFO_COMM;
        msgTypeStrHash[INFO_OTHER] = "other";
        strMsgTypeHash[msgTypeStrHash[INFO_OTHER]] = INFO_OTHER;

        // Инициализация для типа ErrorLevel
        errorLevelStrHash[LEVEL_TRACE] = "trace";
        strErrorLevelHash[errorLevelStrHash[LEVEL_TRACE]] = LEVEL_TRACE;
        errorLevelStrHash[LEVEL_DEBUG] = "debug";
        strErrorLevelHash[errorLevelStrHash[LEVEL_DEBUG]] = LEVEL_DEBUG;
        errorLevelStrHash[LEVEL_INFO] = "info";
        strErrorLevelHash[errorLevelStrHash[LEVEL_INFO]] = LEVEL_INFO;
        errorLevelStrHash[LEVEL_WARN] = "warn";
        strErrorLevelHash[errorLevelStrHash[LEVEL_WARN]] = LEVEL_WARN;
        errorLevelStrHash[LEVEL_ERROR] = "error";
        strErrorLevelHash[errorLevelStrHash[LEVEL_ERROR]] = LEVEL_ERROR;
        errorLevelStrHash[LEVEL_FATAL] = "fatal";
        strErrorLevelHash[errorLevelStrHash[LEVEL_FATAL]] = LEVEL_FATAL;

        strHashesInitialized = true;
    }
}

void ELogger::threadExiting() {

}

std::string ELogger::infoTypeStr(InfoType type) {
    if (msgTypeStrHash.find(type) != msgTypeStrHash.end())
        return msgTypeStrHash[type];
    else
        return "unknown";
}

std::string ELogger::errorLevelStr(ErrorLevel level) {
    if (errorLevelStrHash.find(level) != errorLevelStrHash.end())
        return errorLevelStrHash[level];
    else
        return "unknown";
}

uint32_t ELogger::typeForStr(const std::string &str) {
    if (strMsgTypeHash.find(str) != strMsgTypeHash.end())
        return strMsgTypeHash[str];
    else
        return 0;
}

uint32_t ELogger::errorLevelForStr(const std::string &str) {
    if (strErrorLevelHash.find(str) != strErrorLevelHash.end())
        return strErrorLevelHash[str];
    else
        return 0;
}
