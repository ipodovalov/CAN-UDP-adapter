#include "candevicedriver.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include <elogger.h>

#include <cansocket.h>

CANDeviceDriver::CANDeviceDriver() :
    CANSocket1(NULL), CANSocket2(NULL) 
{
    CANSocket1 = new SocketCAN((char*)"can0") ;
    assert(CANSocket1 != NULL);
    CANSocket2 = new SocketCAN((char*)"can1") ;
    assert(CANSocket2 != NULL);
}

CANDeviceDriver::~CANDeviceDriver() {
    if (CANSocket1 != NULL) {
        delete CANSocket1;
    }
    if (CANSocket2 != NULL) {
        delete CANSocket2;
    }
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
