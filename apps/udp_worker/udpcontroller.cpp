#include "udpcontroller.h"

#include <mqueue.h>
#include <thread>
#include <cassert>
#include <iostream>
#include <ctime>

#include <shmstorage.h>

#include <elogger.h>
#include <embtime.h>

UDPController::UDPController(uint16_t port1, uint16_t port2) :
    device(NULL),
    CANSharedData("can_data"),
    UDPSharedData("udp_data"),
    isRunning(false)
{
    // Создание экземпляра класса Ethernet интерфейса
    device = new EthernetDeviceDriver(port1, port2);
    assert(device != NULL);
}

UDPController::~UDPController() {
    isRunning = false;
}

void UDPController::start(int timeout) {
  uint8_t res;
  can_data_t CANstateRecord;
  udp_data_t UDPstateRecord;
  byte_array UDPDataPort1, UDPDataPort2;
  isRunning = true;

  // Основной цикл программы
  /*
   * Читаются данные CAN из разделяемой памяти, выполняется
   * отправка их по сети, принимаются данные UDP и
   * помещаются также в разделяемую память, для обработки другим 
   * процессом (can_worker). 
   * До тех пор пока не будет вызван метод stop().
   */
  while (isRunning) {
      // Задержка между опросами
      usleep(timeout * 1000UL);

      // Читаю данные CAN из разделяемой памяти и отправляю их по сети
      CANstateRecord = CANSharedData.get();
      res = device->sendData(CANstateRecord);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось отправить данные по сети. Ошибка:" << res;
      }
      
      // Очистка переменной с данными перед приёмом данных по UDP
      UDPDataPort1.clear(); UDPDataPort2.clear();
      
      // Чтение посылки от Мастера
      res = device->getData(UDPDataPort1, UDPDataPort2);

      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось принять данные по сети. Ошибка:" << res;
          continue;
      }

      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Полученные по сети данные 1 порт (" << UDPDataPort1.size() << "байт ):" << UDPDataPort1 ;
      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Полученные по сети данные 2 порт (" << UDPDataPort2.size() << "байт ):" << UDPDataPort2 ;
      
      // TODO сформировать из полученного набора байт структуру
      // udp_data_t для помещения в разделяемую память
      
      // Помещаем сформированную структуру в разделяемую память
      UDPSharedData.set(UDPstateRecord);
  }
}
