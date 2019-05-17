#ifndef CANSOCKET_H_
#define CANSOCKET_H_

#include <stdio.h>
#include <elogger.h>

using namespace std;

//-----------------------------------------------------------------------------
class SocketCAN
{
public:
    SocketCAN(char* CANport=NULL);
    
    bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
    bool CANOpen();
    bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);

protected:
    int   skt;
    char*  _CANport;
};

#endif /* CANSOCKET_H_ */


