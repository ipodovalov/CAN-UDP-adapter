#ifndef CANSOCKET_H_
#define CANSOCKET_H_

#include <stdio.h>
#include <elogger.h>

using namespace std;

//-----------------------------------------------------------------------------
class SocketCAN
{
protected:
    bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
    bool CANOpen();
    bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);

    int   skt;
    char*  _CANport;

public:
    SocketCAN(char* CANport=NULL);
};

#endif /* CANSOCKET_H_ */


