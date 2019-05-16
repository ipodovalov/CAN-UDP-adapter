#include "udpcontroller.h"

#include <mqueue.h>
#include <thread>
#include <cassert>
#include <iostream>
#include <ctime>

#include <shmstorage.h>

#include <elogger.h>
#include <embtime.h>

CANController::CANController() :
    device1(NULL),
    device2(NULL),
    CANSharedData("can_data"),
    UDPSharedData("udp_data"),
    isRunning(false)
{
    // Создание экземпляра класса Ethernet интерфейса
    device = new CANDeviceDriver();
    assert(device != NULL);
}

CANController::~CANController() {
    isRunning = false;
}

//! Основной цикл программы
void UDPController::start(int timeout) {
  uint8_t res;
  can_data_t CANstateRecord;
  udp_data_t UDPstateRecord;
  byte_array CANDataInterface1, CANDataInterface2;
  isRunning = true;

  // Основной цикл программы
  /*
   * Читаются данные UDP из разделяемой памяти, выполняется
   * отправка их по CAN интерфейсам, принимаются данные c CAN
   * интерфейсов и помещаются также в разделяемую память,
   * для обработки другим процессом (udp_worker). 
   * До тех пор пока не будет вызван метод stop().
   */
  while (isRunning) {
      // Задержка между опросами
      usleep(timeout * 1000UL);

      // Читаю данные UDP из разделяемой памяти и отправляю их по CAN
      UDPstateRecord = UDPSharedData.get();
      res = device1->sendData(UDPstateRecord);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось отправить данные по CAN1. Ошибка:" << res;
      }
      
      res = device2->sendData(UDPstateRecord);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось отправить данные по CAN2. Ошибка:" << res;
      }
      
      // Очистка переменной с данными перед приёмом данных по UDP
      CANDataInterface1.clear(); CANDataInterface2.clear();
      
      // Чтение данных с интерфесов CAN
      res = device1->getData(CANDataInterface1);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось получить данные с CAN1. Ошибка:" << res;
          continue;
      }
      res = device2->getData(CANDataInterface2);
      if (res != E_OK) {
          ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось получить данные с CAN2. Ошибка:" << res;
          continue;
      }

      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Полученные по CAN1 данные (" << CANDataInterface1.size() << "байт ):" << CANDataInterface1 ;
      ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Полученные по CAN2 данные (" << CANDataInterface2.size() << "байт ):" << CANDataInterface2 ;
      
      // Формируем структуру can_data_t
      constructCANdataStructure(CANstateRecord, CANDataInterface1, CANDataInterface2) ;
      
      // Помещаем сформированную структуру в разделяемую память
      CANSharedData.set(CANstateRecord);
  } // while
}

//! Внутренний метод, формирующий структуру can_data_t
void CANController::constructCANdataStructure(can_data_t &CANstateRecord, const byte_array &CANDataInterface1, const byte_array &CANDataInterface2) {
//	for(size_t i=0; i<UDPDataPort1.size(); i++) {
//		UDPstateRecord.datagram_port1[i] = UDPDataPort1[i];
//	}
//	UDPstateRecord.datagram_size_port1 = UDPDataPort1.size();
//
//	for(size_t i=0; i<UDPDataPort2.size(); i++) {
//		UDPstateRecord.datagram_port2[i] = UDPDataPort2[i];
//	}
//	UDPstateRecord.datagram_size_port2 = UDPDataPort2.size();
}
