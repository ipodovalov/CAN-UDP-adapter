#ifndef UDPCONTROLLER_H
#define UDPCONTROLLER_H

#include <cstdint>
#include <string>
#include <list>

#include <atomic>

#include <typedefs.h>
#include <shmstorage.h>

#include <ethernetdevicedriver.h>
#include <shmstorage.h>

//! Класс контроллера
class UDPController
{
public:
    /*!
     * \brief Конструктор класса
     */
    UDPController(uint16_t port1, uint16_t port2);

    ~UDPController();
     //! Функция опроса
    /*! Опрос выполняется в вызывающем потоке. Функция блокируется до вызова функции stop из другого потока */
    void start(int timeout);

    //! Прервать цикл
    void stop() { isRunning = false; }


private:
    //! Указатель на экземпляр класса-драйвера МСУ
    EthernetDeviceDriver *device;
    uint32_t port1, port2;
    
    //! Экземпляр класса-хранилища (в разделяемой между приложениями памяти)
    SHMStorage<can_data_t> CANSharedData;

    std::atomic<bool> isRunning;
};

#endif // UDPCONTROLLER_H
