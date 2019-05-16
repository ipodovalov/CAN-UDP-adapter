#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <cstdint>
#include <string>

//extern "C" {
#include <embtime.h>
//}

typedef std::basic_string<uint8_t> byte_array;

typedef float float32_t;

//! Коды ошибок
enum ERROR_CODE : uint8_t {
    E_OK                            = 0x00, /*!< Успешное завершение */
    E_ALLOC_ERROR                   = 0x01, /*!< Ошибка выделения памяти */
    E_TIMEOUT                       = 0x02,
    E_NULL                          = 0x03,
    E_DEVICE_NOT_PRESENT            = 0x04,
    E_CHECKSUM_ERROR                = 0x05,
    E_CLOCK_ERROR                   = 0x06,
    E_FAIL                          = 0x07,
    E_INVALID_DATA                  = 0x08,
    E_LEN_ERROR                     = 0x09,
    E_BUFFER_OVERFLOW               = 0x0A,
    E_BUFFER_UNDERFLOW              = 0x0B,
    E_INVALID_LENGTH                = 0x0C,
    E_SYNC_ERROR                    = 0x0D,
    E_LOCK_ERROR                    = 0x0E,
    E_INVALID_ADDRESS               = 0x0F,
    E_TOO_LATE                      = 0x10,
    E_DEVICE_BUSY                   = 0x11,
    E_PORT_CLOSED                   = 0x12,
    E_WRITE_ERROR                   = 0x13,
    E_READ_ERROR                    = 0x14,
    E_INVALID_ANSWER                = 0x15,
    E_DB_OPEN_ERROR                 = 0x16,
    E_DB_ERROR                      = 0x17,
    E_MORE_BYTES                    = 0x18,
    E_SOCKET_ERROR                  = 0x19,
    E_HOSTNAME_ERROR                = 0x1A,
    E_CONNECT_ERROR                 = 0x1B
};

#define BUF_SIZE 1500                        // длина UDP пакета

typedef struct __attribute__ ((__packed__)) {
    embtime_t   time_stamp;
	int16_t     zerkalo[20];
	uint16_t    zer_size;
} can_data_t;

typedef struct __attribute__ ((__packed__)) {
    embtime_t   time_stamp;
	uint8_t     datagram_port1[BUF_SIZE] ;  // свежий UDP-пакет (1 порт)
	uint16_t    datagram_size_port1 ;		// размер принятых данных
	uint8_t     datagram_port2[BUF_SIZE] ;  // свежий UDP-пакет (2 порт)
	uint16_t    datagram_size_port2 ;		// размер принятых данных
} udp_data_t;

#endif // TYPEDEFS_H
