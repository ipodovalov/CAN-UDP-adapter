#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include <cstdint>
#include <string>
#include <list>

#include <atomic>

#include <typedefs.h>

#include <candevicedriver.h>
#include <shmstorage.h>

//! Класс контроллера
class CANController
{
public:
    /*!
     * \brief Конструктор класса
     */
    CANController();

    ~CANController();
     //! Функция опроса
    /*! Опрос выполняется в вызывающем потоке. Функция блокируется до вызова функции stop из другого потока */
    void start(int timeout);

    //! Прервать цикл
    void stop() { isRunning = false; }

protected:
	void constructCANdataStructure(can_data_t &CANstateRecord, const can_frame &CANData1, const can_frame &CANData2) ;

private:
    //! Указатель на экземпляры класса-драйвера CAN-интерфейса
    CANDeviceDriver *device1, *device2;
    
    //! Экземпляр класса-хранилища (в разделяемой между приложениями памяти)
    SHMStorage<can_data_t> CANSharedData;
    SHMStorage<udp_data_t> UDPSharedData;

    std::atomic<bool> isRunning;
};

#endif // CANCONTROLLER_H
