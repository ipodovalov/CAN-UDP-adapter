#include "udpcontroller.h"

#include <mqueue.h>
#include <thread>
#include <cassert>
#include <iostream>
#include <ctime>

#include <shmstorage.h>

#include <elogger.h>
#include <embtime.h>

UDPController::UDPController() :
    device(NULL),
    msuSharedData("usta_diag_data"),
    isRunning(false)
{
    // Создание экземпляра класса Ethernet интерфейса
    device = new EthernetDeviceDriver();
    assert(device != NULL);
}

UDPController::~UDPController() {
    isRunning = false;
}

ERROR_CODE UDPController::openPort(uint16_t port1, uint16_t port2) {
    return device->openPort(port1, port2);
}

void UDPController::start(int timeout) {
  uint8_t res;
  usta_diagnostic_data_t msuStateRecord;
  byte_array masterData;
  
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
      
      msuStateRecord = msuSharedData.get();
      
      res = device->sendData(msuStateRecord);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось отправить данные Мастеру. Ошибка:" << res;
      }
  }
}
