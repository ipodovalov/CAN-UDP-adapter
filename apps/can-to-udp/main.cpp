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
#include <msupollcontroller.h>
#include <inireader.h>
#include <daemon.h>

using namespace std;

//! Параметры, задаваемые из командной строки
struct globalArgs_t {
    uint32_t                device_speed;           // -s
    string                  device_path;
    uint32_t                inter_byte_timeout;     // межбайтовый таймаут для операции чтения
    uint32_t                read_timeout;           // таймаут появления данных в приемном буфере UART
    int                     stop_bit;
    int                     timeout;
    int                     poll_count;
    std::string             configPath;             // --config
    std::string             pidFilePath;            // --pidfile
} globalArgs;

//! Описание допустимых опций командной строки
static const char *opts_string = "bs:d:t:";

//! Описание допустимых опций командной строки
static const struct option long_opts[] = {
    { "speed",      required_argument,  NULL,   's' },
    { "dev",        required_argument,  NULL,   'd' },
    { "timeout",    required_argument,  NULL,   't' },
    { "config",     required_argument,  NULL,   0 },
    { "pidfile",    required_argument,  NULL,   0 },
    { "daemonize",  no_argument,        NULL,   'b' },
    { "help",       no_argument,        NULL,   0 },
    { NULL,         no_argument,        NULL,   0 }
};

void print_help() {
  printf("Это страница помощи для работы с программой can_udp_adapter\n"
         "Список опций:\n"
         "    -h          --help              отображает то, что вы видите сейчас\n"
         "    -b          --daemonize         запуск в режиме демона\n"
         "    -s speed    --speed speed       установить скорость обмена (по умолчанию 115200)\n"
         "    -d device   --dev device        установить путь к устройству (по умолчанию /dev/ttySU1)\n"
         "    -t timeout  --timeout timeout   установить периодичность опроса устройства (в милисекундах)\n"
         "                --config file_path  установить путь к config файлу (default /etc/can_udp_adapter.conf)\n"
         "                --pidfile file_path установить путь к pid файлу (default /tmp/run/comsndr.pid)\n"
         );
}

MSUPollController *pollController = NULL;

uint8_t sigcount = 0;

//! Обработчик сигналов операционной системы (в том числе сигнала по Ctrl + C)
void sigint_handler(int signal) {
    if (signal == 2) {
        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "Получен сигнал SIGINT. Завершаем...";
        if (sigcount++ > 0) {
            exit(3);
        }
        if (pollController != NULL) {
            pollController->stop();
        } else {
            exit(2);
        }
    }
}

void initLogger() {
    ELogger::initLogger();
    ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "mpm_comsndr запущена";
}

int main(int argc, char *argv[]) {
    int opt = 0;
    int option_index;

    // Задание значений опций по умолчанию
    globalArgs.device_path = "/dev/ttyO4";
    globalArgs.device_speed = 19200;
    globalArgs.inter_byte_timeout = 10;
    globalArgs.read_timeout = 50;
    globalArgs.timeout = 50;
    globalArgs.configPath = "/etc/mpm/mpm.conf";
    globalArgs.pidFilePath = "/tmp/run/comsndr.pid";
    globalArgs.stop_bit = 1;

    // Обработка опций командной строки
    opt = getopt_long(argc, argv, opts_string, long_opts, &option_index);
    while (opt != -1) {
        switch( opt ) {
        case 's':
            globalArgs.device_speed = atoi(optarg);
            break;
        case 'd':
            globalArgs.device_path = optarg;
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
        // Config file read
        globalArgs.device_path =            config.get("comsndr", "dev_path", globalArgs.device_path);
        globalArgs.device_speed =           config.getUInt("comsndr", "dev_speed", globalArgs.device_speed);
        globalArgs.stop_bit =               config.getUInt("comsndr", "stop_bit", globalArgs.stop_bit);
        globalArgs.inter_byte_timeout =     config.getUInt("comsndr", "inter_byte_timeout", globalArgs.inter_byte_timeout);
        globalArgs.read_timeout =           config.getUInt("comsndr", "read_timeout", globalArgs.read_timeout);
        globalArgs.timeout =                config.getUInt("comsndr", "poll_delay_msec", globalArgs.timeout);
        globalArgs.pidFilePath =            config.get("comsndr", "pidfile", globalArgs.pidFilePath);
    } else {
        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_WARN) << "Ошибка чтения mpm.conf файла:" << config.parseResult();
    }

    Daemon::checkInstance(globalArgs.pidFilePath);

    uint8_t res;

    // Создание экземпляра контроллера опроса МСУ
    pollController = new MSUPollController();
    assert(pollController != NULL);

    // Открытие порта устройства МСУ
    res = pollController->openPort(globalArgs.device_path, globalArgs.device_speed, globalArgs.inter_byte_timeout, globalArgs.read_timeout, globalArgs.stop_bit);

    if (res != E_OK) {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось открыть порт " << globalArgs.device_path << "; ERROR:" << res;
        delete pollController;
        return 1;
    }
  
    pollController->start(globalArgs.timeout, globalArgs.read_timeout);

    if (pollController != NULL) {
        delete pollController;
    }

    return 0;
}
