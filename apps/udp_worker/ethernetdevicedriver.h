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
    EthernetDeviceDriver(uint16_t port1, uint16_t port2);

    ~EthernetDeviceDriver();

    /*!
     * \brief Отправка в сеть полученных по CAN-шине данных
     * \param CANData текущие данные для отправки
     * \return код ошибки
     */
    ERROR_CODE sendData(const can_data_t CANData);

    /*!
     * \brief Слушаем порт UDP
     * \param UDPDataPort1 массив байт полученный c 1 порта
     * \param UDPDataPort2 массив байт полученный cо 2 порта
     * \return код ошибки
     */
    ERROR_CODE getData(byte_array &UDPDataPort1, byte_array &UDPDataPort2);

protected:
   
private:
    //! Указатель на экземпляр класса для работы с Ethernet
    DatagramSocket *UDPSocket1;
    DatagramSocket *UDPSocket2;
};

#endif // MSUDEVICEDRIVER_H
