#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <print_log.h>

#define UPS_TCP_PORT 5454
struct sockaddr_in servaddrUDP, cliaddrUDP ;				// структура для хранения адреса

// функция создания сокета
int make_socket( int *udpSocket, socklen_t *len_addr_server, socklen_t *len_addr_client )
{
	int sk ;												// дискриптор сокета
	const int optvalue = 1;									// опция сокета
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	sk = socket(AF_INET, SOCK_DGRAM, 0) ;					// создаем сокет UDP, тип пакета датаграммы
	
	if(sk < 0)
	{
		print_log("Ошибка при создании сокета UDP: %d", errno) ;
		return -1 ;
	}

	memset( &servaddrUDP, 0, sizeof(servaddrUDP) ) ;		// обнуляем структуру и заполняем структуру:
	servaddrUDP.sin_family = AF_INET ;						// тип сокета - UDP
	servaddrUDP.sin_port = htons( UPS_TCP_PORT ) ;		    // порт
	const char *ipaddress = "192.168.0.255";				// широковещательный адрес
	inet_pton(AF_INET, ipaddress, &servaddrUDP.sin_addr);

	memset( &cliaddrUDP, 0, sizeof(cliaddrUDP) ) ;			// обнуляем структуру и заполняем структуру:
	cliaddrUDP.sin_family = AF_INET ;						// тип сокета - UDP
	cliaddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
	cliaddrUDP.sin_port = htons( UPS_TCP_PORT ) ;		    // порт

	*len_addr_server = sizeof(cliaddrUDP) ;
	*len_addr_client = sizeof(servaddrUDP) ;
	
	setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &optvalue, sizeof(servaddrUDP));// разрешить широковещательную передачу
	setsockopt(sk, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	
	// Узнать параметры сокета (для справки) 163840 для МПМ
/*	int option = 0;
	getsockopt(sk, SOL_SOCKET, SO_RCVBUF, &option, len_addr_server);
	print_log("\nSO_RCVBUF=%d\n", option);
*/
	
	if( bind( sk,(struct sockaddr *)&cliaddrUDP,sizeof(cliaddrUDP)) < 0 )		// выполняем привязку сокета к физическому адресу сетевого интерфейса (описанного в структуре sipx)
	{
		print_log("Ошибка при попытке создать соединение: %d", errno) ;
		close ( sk ) ;
		return -1 ;
	}

	*udpSocket = sk ;							// возвращаем значение дискриптора сокета в вызывающую функцию по адресу переменной udpSocket
	return 0 ;								// функция возвращает ноль, в случае успеха всего выше выполненного
}
