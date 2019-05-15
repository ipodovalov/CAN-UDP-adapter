#ifndef ETHERNETDEVICEDRIVER_H
#define ETHERNETDEVICEDRIVER_H

#include <cstdint>
#include <string>

#include <typedefs.h>
#include <datagramsocket.h>

//! Класс-драйвер для работы с Ethernet
class EthernetDeviceDriver
{
public:
    //! Структура с описанием формата запроса данных от МСУ (содержит текущее состояние GPS и топливных датчиков)
    typedef struct __attribute__ ((__packed__)) {
		uint8_t one_byte_zerkalo[40];
    } msu_device_request_data_t;

    EthernetDeviceDriver(uint16_t port1, uint16_t port2);

    ~EthernetDeviceDriver();

    /*!
     * \brief Отправка в сеть полученных по CAN-шине данных
     * \param CANData текущие данные для отправки
     * \return код ошибки
     */
    ERROR_CODE sendData(const usta_diagnostic_data_t &CANData);

    /*!
     * \brief Считать ответ от Мастера
     * \param responsе массив байт, содержащий ответ.
     * \return код ошибки
     */
    ERROR_CODE getData(byte_array &response, uint32_t read_timeout);

protected:
   
private:
    //! Указатель на экземпляр класса для работы с Ethernet
    DatagramSocket *UDPSocket1;
    DatagramSocket *UDPSocket2;
};

#endif // MSUDEVICEDRIVER_H
