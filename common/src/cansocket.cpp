#include "cansocket.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

//*****************************************************************************
SocketCAN::SocketCAN(char* CANport) 
{
    static char defaultCANport[] = "can0";

    if (CANport != NULL)
        _CANport = CANport;
    else
        _CANport = defaultCANport;
}


//*****************************************************************************
bool SocketCAN::CANOpen() {
    struct ifreq ifr;
    struct sockaddr_can addr;
    int flags;

    //----  Создаёт сокет к CAN-интерфейсу
    skt = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(skt < 0) {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось открыть сокет к интерфейсу:" << _CANport;
        return (false);
        }

    strncpy(ifr.ifr_name, _CANport, (sizeof(ifr.ifr_name)-1));
    ifr.ifr_name[sizeof(ifr.ifr_name)] = '\0';

    if (ioctl(skt, SIOCGIFINDEX, &ifr) < 0) {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не удалось настроить CAN (ioctl) :" << ifr.ifr_name;
        return (false);
        }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(skt, (struct sockaddr *)&addr, sizeof(addr)) < 0)  {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не биндится CAN сокет" ;
        return (false);
        }

    //----- Установка сокета как неблокирующего
    flags = fcntl(skt, F_GETFL, 0);
    if (flags < 0) {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не смог прочитать настройки сокета" ;
        return (false);
        }

    if (fcntl(skt, F_SETFL, flags | O_NONBLOCK) < 0) {
        ELOG(ELogger::INFO_DEVICE, ELogger::LEVEL_ERROR) << "Не смог установить настройку (O_NONBLOCK) сокета" ;
        return (false);
        }

    return true;
}

//*****************************************************************************
bool SocketCAN::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
   struct can_frame frame_wr;

   frame_wr.can_id  = id | CAN_EFF_FLAG;
   frame_wr.can_dlc = len;
   memcpy(frame_wr.data, buf, 8);

   return (write(skt, &frame_wr, sizeof(frame_wr)) == sizeof(frame_wr));
}

//*****************************************************************************
bool SocketCAN::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf) {
    struct can_frame frame_rd;
    struct timeval tv = {0, 0};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(skt, &fds);

    if (select((skt + 1), &fds, NULL, NULL, &tv) < 0)
        return false;

    if (FD_ISSET(skt, &fds)) {
        if (read(skt, &frame_rd, sizeof(frame_rd)) > 0) {
            memcpy(buf, frame_rd.data, 8);
            len = frame_rd.can_dlc;
            id  = frame_rd.can_id;
            return true;
            }
        }
    return false;
}


