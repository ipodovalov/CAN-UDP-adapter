#ifndef CANDEVICEDRIVER_H
#define CANDEVICEDRIVER_H

#include <cstdint>
#include <string>

#include <typedefs.h>
#include "cansocket.h"

//! Класс-драйвер для работы с CAN
class CANDeviceDriver
{
public:
    CANDeviceDriver();

    ~CANDeviceDriver();

    /*!
     * \brief Отправка в сеть полученных по UDP данных
     * \param UDPData текущие данные для отправки
     * \return код ошибки
     */
    ERROR_CODE sendData(const udp_data_t UDPData);

    /*!
     * \brief Слушаем интерфейс CAN
     * \param CANDataInterface массив байт полученный по CAN
     * \return код ошибки
     */
    ERROR_CODE getData(byte_array &CANDataInterface);
  
private:
    //! Указатель на экземпляр класса для работы с CAN
    SocketCAN *CANSocket1;
    SocketCAN *CANSocket2;
};

#endif // CANDEVICEDRIVER_H
