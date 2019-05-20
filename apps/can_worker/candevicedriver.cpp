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

ERROR_CODE CANDeviceDriver::sendData(const udp_data_t UDPData, int interface_num) {
    byte_array packet;      // Байтовый буфер данных для отправки по CAN
    int i = 0; bool rez = false;
    unsigned long id = 0x42;
    unsigned char len = 2;
    unsigned char buf[8] ;
    // Добавление данных тела пакета в буфер для отправки
    switch(interface_num) {
        case 1:
            // Отправка буфера в нужный CAN-интерфейс
            id = 0x123;
            do {
                memcpy(buf, UDPData.datagram_port2+i, 8);
                rez = CANSocket->CANSendFrame(id, len, buf);
                i+=8;
            } while(i<BUF_SIZE);
            packet.append(reinterpret_cast<const uint8_t *>(&UDPData.datagram_port1), UDPData.datagram_size_port1);
            ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Отправленные по CAN1 данные (" << packet.size() << "байт ):" << packet ;
            break;
        case 2:
            // Отправка буфера в нужный CAN-интерфейс
            id = 0x321;
            do {
                memcpy(buf, UDPData.datagram_port2+i, 8);
                rez = CANSocket->CANSendFrame(id, len, buf);
                i+=8;
            } while(i<BUF_SIZE);
            packet.append(reinterpret_cast<const uint8_t *>(&UDPData.datagram_port2), UDPData.datagram_size_port2);
            ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_TRACE) << "Отправленные по CAN2 данные (" << packet.size() << "байт ):" << packet ;
            break;
        default:
            break;
    }
       
    if(!rez) {
        return E_OK;
    } else {
        return E_WRITE_ERROR;
    }
}
