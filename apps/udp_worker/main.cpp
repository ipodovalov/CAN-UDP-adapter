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
    uint16_t                udp_port1;
    uint16_t                udp_port2;
    int                     timeout;
    std::string             configPath;
    std::string             pidFilePath;
} globalArgs;

//! Описание допустимых опций командной строки
static const char *opts_string = "b1:2:t:";

//! Описание допустимых опций командной строки
static const struct option long_opts[] = {
    { "port1",      required_argument,  NULL,   '1' },
    { "port2",      required_argument,  NULL,   '2' },
    { "timeout",    required_argument,  NULL,   't' },
    { "config",     required_argument,  NULL,   0 },
    { "pidfile",    required_argument,  NULL,   0 },
    { "daemonize",  no_argument,        NULL,   'b' },
    { "help",       no_argument,        NULL,   0 },
    { NULL,         no_argument,        NULL,   0 }
};

void print_help() {
  printf("Это страница помощи для работы с программой udp_worker.\n"
         "Программа берёт из разделяемой памяти структуру, полученную\n"
         "по интерфейсу CAN, отправляет эту структуру по заданному порту\n"
         "UDP, кроме того получает по UDP данные и помещает эти данные\n"
         "также в разделяемую память, для обработки другим процессом\n"
         "Список опций:\n"
         "    -h          --help              отображает то, что вы видите сейчас\n"
         "    -b          --daemonize         запуск в режиме демона\n"
         "    -1 port1    --port1 port1       задать номер порта UDP соответствующий CAN1-интерфейсу\n"
         "    -2 port2    --port2 port2       задать номер порта UDP соответствующий CAN2-интерфейсу\n"
         "    -t timeout  --timeout timeout   установить периодичность опроса устройства (в милисекундах)\n"
         "                --config file_path  путь к config файлу (default /etc/can_udp_converter.conf)\n"
         "                --pidfile file_path установить путь к pid файлу (default /tmp/run/udp_worker.pid)\n"
         );
}

UDPController *Controller = NULL;

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

void initLogger() {
    ELogger::initLogger();
    ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "udp_worker запущена";
}

int main(int argc, char *argv[]) {
    int opt = 0;
    int option_index;

    // Задание значений опций по умолчанию
    globalArgs.udp_port1 = 5555;
    globalArgs.udp_port2 = 5454;
    globalArgs.timeout = 50;
    globalArgs.configPath = "/etc/can_udp_converter.conf";
    globalArgs.pidFilePath = "/tmp/run/udp_worker.pid";

    // Обработка опций командной строки
    opt = getopt_long(argc, argv, opts_string, long_opts, &option_index);
    while (opt != -1) {
        switch( opt ) {
        case '1':
            globalArgs.udp_port1 = atoi(optarg);
            break;
        case '2':
            globalArgs.udp_port2 = atoi(optarg);
            break;
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

    signal(SIGINT, sigint_handler);

    initLogger();

    INIReader config(globalArgs.configPath);

    if (config.parseResult() == 0) {
        globalArgs.udp_port1 =           config.getUInt("udp_worker", "udp_port1", globalArgs.udp_port1);
        globalArgs.udp_port2 =               config.getUInt("udp_worker", "udp_port2", globalArgs.udp_port2);
        globalArgs.timeout =                config.getUInt("udp_worker", "poll_delay_msec", globalArgs.timeout);
        globalArgs.pidFilePath =            config.get("udp_worker", "pidfile", globalArgs.pidFilePath);
    } else {
        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_WARN) << "Ошибка чтения can_udp_converter.conf файла:" << config.parseResult();
    }

    Daemon::checkInstance(globalArgs.pidFilePath);

    uint8_t res;

    // Создание экземпляра контроллера
    Controller = new UDPController(globalArgs.udp_port1, globalArgs.udp_port2);
    assert(Controller != NULL);
 
    Controller->start(globalArgs.timeout);

    if (Controller != NULL) {
        delete Controller;
    }

    return 0;
}
