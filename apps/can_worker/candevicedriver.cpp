#include "candevicedriver.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include <elogger.h>

#include <cansocket.h>

CANDeviceDriver::CANDeviceDriver(char* can_interface_name) :
    CANSocket(NULL)
{
    CANSocket = new SocketCAN(can_interface_name) ;
    assert(CANSocket != NULL);
}

CANDeviceDriver::~CANDeviceDriver() {
    if (CANSocket != NULL) {
        delete CANSocket;
    }
}

bool CANDeviceDriver::CANOpen() {
    return CANSocket->CANOpen();
}

ERROR_CODE CANDeviceDriver::getData(can_frame &CANData)
{
    unsigned long id;
    unsigned char len;
    unsigned char buf[8];
    int rez = CANSocket->CANGetFrame(id, len, buf);
    CANData.can_id = id;
    CANData.can_dlc = len;
    memcpy(CANData.data, buf, 8);
    if( !rez ) {
        return E_OK;
    } else {
        return E_READ_ERROR;
    }
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
