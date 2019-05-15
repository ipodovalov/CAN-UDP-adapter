#ifndef UDPCONTROLLER_H
#define UDPCONTROLLER_H

#include <cstdint>
#include <string>
#include <list>

#include <atomic>

#include <typedefs.h>
#include <shmstorage.h>

#include <msudevicedriver.h>
#include <shmstorage.h>

//! Класс контроллера
class MSUPollController
{
public:
    /*!
     * \brief Конструктор класса
     */
    UDPController();

    ~UDPController();

    /*!
     * \brief функция открытия порта
     * \param port1 номер 1 UDP порта
     * \param port2 номер 2 UDP порта
     * \return код ошибки
     */
    ERROR_CODE openPort(std::string port, uint32_t speed, uint32_t inter_byte_timeout, uint32_t read_timeout, int stop_bit);

    //! Функция опроса
    /*! Опрос выполняется в вызывающем потоке. Функция блокируется до вызова функции stop из другого потока */
    void start(int timeout);

    //! Прервать цикл
    void stop() { isRunning = false; }


private:
    //! Экземпляр класса-хранилища (в разделяемой между приложениями памяти)
    SHMStorage<usta_diagnostic_data_t> msuSharedData;

    std::atomic<bool> isRunning;
};

#endif // UDPCONTROLLER_H
