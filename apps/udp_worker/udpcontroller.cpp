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
  can_data_t Record;
  can_data_t CANstateRecord;
  byte_array masterData;
  int read_timeout = 1;
  isRunning = true;

  // Чтение запросов от Мастера пока не будет вызван метод stop()
  /*
   * Cчитывается посылка от Мастера и формируется структура
   * с ответом, в котором находятся диагностические данные
   */
  while (isRunning) {
      // Задержка между опросами
      usleep(timeout * 1000UL);

      // Очистка переменной с данными перед чтением запроса
      masterData.clear();
      
     // Чтение посылки от Мастера
      res = device->getData(masterData, read_timeout);

      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось извлечь полезные данные из пакета от Мастера. Ошибка:" << res;
          continue;
      }

      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Полезные данные Мастера (" << masterData.size() << "байт ):" << masterData ;
      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Данные Мастера корректны. Размер:" << masterData.size() << "байт.";
      
      CANstateRecord = CANSharedData.get();
      
      res = device->sendData(CANstateRecord);
      
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось отправить данные по сети. Ошибка:" << res;
      }
  }
}
