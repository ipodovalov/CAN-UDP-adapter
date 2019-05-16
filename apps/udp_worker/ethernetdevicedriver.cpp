#include "ethernetdevicedriver.h"

#include <iostream>
#include <cassert>
#include <cstring>

#include <elogger.h>

EthernetDeviceDriver::EthernetDeviceDriver(uint16_t port1, uint16_t port2) :
    UDPSocket1(NULL), UDPSocket2(NULL)
{
    UDPSocket1 = new DatagramSocket(port1, (char*)"255.255.255.255", TRUE, TRUE); ;
    assert(UDPSocket1 != NULL);
    UDPSocket2 = new DatagramSocket(port2, (char*)"255.255.255.255", TRUE, TRUE); ;
    assert(UDPSocket2 != NULL);
}

EthernetDeviceDriver::~EthernetDeviceDriver() {
    if (UDPSocket1 != NULL) {
        delete UDPSocket1;
    }
    if (UDPSocket2 != NULL) {
        delete UDPSocket2;
    }
}

ERROR_CODE EthernetDeviceDriver::getData(byte_array &UDPDataPort1, byte_array &UDPDataPort2)
{
	char temp[BUF_SIZE];
	long ret;
	
	ret = UDPSocket1->receive(temp, BUF_SIZE);
	if(ret >= 0) {
	    for (size_t i = 0; i < ret; i++){
            UDPDataPort1.push_back(temp[i]);
		}
	}
	
	ret = UDPSocket2->receive(temp, BUF_SIZE);
	if(ret >= 0) {
	    for (size_t i = 0; i < ret; i++){
            UDPDataPort2.push_back(temp[i]);
		}
	}
	
    return E_OK;
}

ERROR_CODE EthernetDeviceDriver::sendData(const can_data_t &CANData) {
    byte_array packet;      // Байтовый буфер данных для отправки в сеть
    
    ssize_t bytesWritten;

    // Добавление данных тела пакета в буфер для отправки
    packet.append(reinterpret_cast<const uint8_t *>(&CANData), sizeof(CANData));

	// Тут определяем в какой порт отправлять данные

    // Отправка сформированного буфера в нужный UDP порт 
    bytesWritten = UDPSocket1->send((char*)packet.c_str(), packet.size());
    bytesWritten = UDPSocket2->send((char*)packet.c_str(), packet.size());
	
	ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Отправлен пакет (" << packet.size() << " байт ):" << packet;

    if ((bytesWritten >= 0) && (bytesWritten == packet.size())) {
        return E_OK;
    } else {
        return E_WRITE_ERROR;
    }
}
