#include "candevicedriver.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include <elogger.h>

CANDeviceDriver::CANDeviceDriver() :
//    UDPSocket1(NULL), UDPSocket2(NULL) // тут будут другие устройства
{
//    UDPSocket1 = new DatagramSocket(port1, (char*)"255.255.255.255", TRUE, TRUE); ;
//    assert(UDPSocket1 != NULL);
//    UDPSocket2 = new DatagramSocket(port2, (char*)"255.255.255.255", TRUE, TRUE); ;
//    assert(UDPSocket2 != NULL);
}

CANDeviceDriver::~CANDeviceDriver() {
//    if (UDPSocket1 != NULL) {
//        delete UDPSocket1;
//    }
//    if (UDPSocket2 != NULL) {
//        delete UDPSocket2;
//    }
}

ERROR_CODE CANDeviceDriver::getData(byte_array &CANDataInterface)
{
	return E_OK;
}

ERROR_CODE CANDeviceDriver::sendData(const udp_data_t CANData) {
//    byte_array packet;      // Байтовый буфер данных для отправки по CAN
/*    
    ssize_t bytesWritten;

    // Добавление данных тела пакета в буфер для отправки
    packet.append(reinterpret_cast<const uint8_t *>(&CANData), sizeof(CANData));

	//! TODO написать блок, где разбираем полученные по CAN данные
	//! и определяем в какой порт отправлять данные по UDP

    // Отправка сформированного буфера в нужный UDP порт 
    bytesWritten = UDPSocket1->send((char*)packet.c_str(), packet.size());
    bytesWritten = UDPSocket2->send((char*)packet.c_str(), packet.size());
	
    if ((bytesWritten >= 0) && (bytesWritten == packet.size())) {
        return E_OK;
    } else {
        return E_WRITE_ERROR;
    }
* */
	return E_OK;
}
