#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <string>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <elogger.h>
#include <udpcontroller.h>
#include <inireader.h>
#include <daemon.h>

using namespace std;

//! Параметры, задаваемые из командной строки
struct globalArgs_t {
    int                     timeout;
    std::string             configPath;
    std::string             pidFilePath;
} globalArgs;

//! Описание допустимых опций командной строки
static const char *opts_string = "b1:2:t:";

static const struct option long_opts[] = {
    { "timeout",    required_argument,  NULL,   't' },
    { "config",     required_argument,  NULL,   0 },
    { "pidfile",    required_argument,  NULL,   0 },
    { "daemonize",  no_argument,        NULL,   'b' },
    { "help",       no_argument,        NULL,   0 },
    { NULL,         no_argument,        NULL,   0 }
};

//! Печать страницы помощи
void print_help() {
  printf("Это страница помощи для работы с программой can_worker.\n"
         "Программа берёт из разделяемой памяти структуру, полученную\n"
         "по UDP, отправляет эту структуру по заданному CAN-интерфейсу\n"
         "кроме того получает по СAN данные и помещает эти данные\n"
         "также в разделяемую память, для обработки другим процессом\n"
         "Список опций:\n"
         "    -h          --help              отображает то, что вы видите сейчас\n"
         "    -b          --daemonize         запуск в режиме демона\n"
         "    -t timeout  --timeout timeout   установить периодичность опроса устройства (в милисекундах)\n"
         "                --config file_path  путь к config файлу (default /etc/can_udp_converter.conf)\n"
         "                --pidfile file_path установить путь к pid файлу (default /tmp/run/udp_worker.pid)\n"
         );
}

CANController *Controller = NULL;

uint8_t sigcount = 0;

//! Обработчик сигналов операционной системы (в том числе сигнала по Ctrl + C)
void sigint_handler(int signal) {
    if (signal == 2) {
        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "Получен сигнал SIGINT. Завершаем...";
        if (sigcount++ > 0) {
            exit(3);
        }
        if (Controller != NULL) {
            Controller->stop();
        } else {
            exit(2);
        }
    }
}

//! Логирование
void initLogger() {
    ELogger::initLogger();
    ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "Программа can_worker запущена";
}

//! Функция main()
int main(int argc, char *argv[]) {
    int opt = 0;
    int option_index;

    // Задание значений опций по умолчанию
    globalArgs.timeout = 50;
    globalArgs.configPath = "/etc/can_udp_converter.conf";
    globalArgs.pidFilePath = "/tmp/run/can_worker.pid";

    // Обработка опций командной строки
    opt = getopt_long(argc, argv, opts_string, long_opts, &option_index);
    while (opt != -1) {
        switch( opt ) {
        case 't':
            globalArgs.timeout = atoi(optarg);
            if (globalArgs.timeout < 0) {
                globalArgs.timeout = 0;
            }
            break;
        case 'b':
            Daemon::daemonize();
            break;
        case 0:
            if (strcmp("help", long_opts[option_index].name ) == 0) {
                print_help();
                return 0;
            }

            if (strcmp("config", long_opts[option_index].name) == 0) {
                globalArgs.configPath = optarg;
            }

            if (strcmp("pidfile", long_opts[option_index].name) == 0) {
                globalArgs.pidFilePath = optarg;
            }
            break;
        default:
            /* сюда попасть невозможно. */
            break;
        }
        opt = getopt_long(argc, argv, opts_string, long_opts, &option_index);
    }

	// Подменяем обработчик сигналов
    signal(SIGINT, sigint_handler);

    initLogger();

	// Читаем конфигурационный файл
    INIReader config(globalArgs.configPath);

    if (config.parseResult() == 0) {
        globalArgs.timeout =                config.getUInt("can_worker", "poll_delay_msec", globalArgs.timeout);
        globalArgs.pidFilePath =            config.get("can_worker", "pidfile", globalArgs.pidFilePath);
    } else {
        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_WARN) << "Ошибка чтения can_udp_converter.conf файла:" << config.parseResult();
    }

	// Процесс в системе должен быть только один
    Daemon::checkInstance(globalArgs.pidFilePath);

    uint8_t res;

    // Создание экземпляра контроллера
    Controller = new CANController();
    assert(Controller != NULL);

	// Запуск основного цикла
    Controller->start(globalArgs.timeout);

    if (Controller != NULL) {
        delete Controller;
    }

    return 0;
    
} // main()
