#include <datagramsocket.h>

DatagramSocket::DatagramSocket(int port, char* address, bool broadcast, bool reusesock)
{
    sockaddr_in addr;
    sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

    //set up bind address
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    //set up address to use for sending
    memset(&outaddr,0,sizeof(outaddr));
    outaddr.sin_family = AF_INET;
    outaddr.sin_addr.s_addr = inet_addr(address);
    outaddr.sin_port = htons(port);

    int OptVal = 1;

    if (broadcast)
        retval = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &OptVal, sizeof(OptVal));

    if (reusesock)
        retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &OptVal, sizeof(OptVal));

    retval = bind(sock,(struct sockaddr *)&addr,sizeof(addr));
}

DatagramSocket::~DatagramSocket()
{
    close(sock);
}

int DatagramSocket::getAddress(const char * name, char * addr)
{
    struct hostent *hp;
    if ((hp = gethostbyname(name)) == NULL) return (0);
    strcpy(addr, inet_ntoa( *( struct in_addr*)( hp->h_addr)));
    return (1);
}

const char* DatagramSocket::getAddress(const char * name)
{
    struct hostent *hp;
    if ((hp = gethostbyname(name)) == NULL) return (0);
    strcpy(ip, inet_ntoa( *( struct in_addr*)( hp->h_addr)));
    return ip;
}

long DatagramSocket::receive(char* msg, int msgsize)
{
    struct sockaddr_in sender;
    socklen_t sendersize = sizeof(sender);
    int retval = recvfrom(sock,msg,msgsize,0, (struct sockaddr *)&sender, &sendersize);
    strcpy(received,inet_ntoa(sender.sin_addr));
    return retval;
}

char* DatagramSocket::received_from()
{
    return received;
}

long DatagramSocket::send(const char* msg, int msgsize)
{
    return sendto(sock, msg, msgsize, 0, (struct sockaddr *)&outaddr,sizeof(outaddr));
}

long DatagramSocket::sendTo(const char* msg, int msgsize, const char* addr)
{
    outaddr.sin_addr.s_addr = inet_addr(addr);
    return sendto(sock, msg, msgsize, 0, (struct sockaddr *)&outaddr,sizeof(outaddr));
}
