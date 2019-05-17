#ifndef CANDEVICEDRIVER_H
#define CANDEVICEDRIVER_H

#include <cstdint>
#include <string>

#include <typedefs.h>
#include <cansocket.h>

//! Класс-драйвер для работы с CAN
class CANDeviceDriver
{
public:
    CANDeviceDriver(char* can_interface_name);

    ~CANDeviceDriver();

    /*!
     * \brief Открывает нам CAN-интерфейс
     * \return true - если удалось
     */
    bool CANOpen();

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
    ERROR_CODE getData(can_frame &CANData);
  
private:
    //! Указатель на экземпляр класса для работы с CAN
    SocketCAN *CANSocket;
};

#endif // CANDEVICEDRIVER_H
