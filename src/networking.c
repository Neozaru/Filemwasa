
	#include <stdio.h>

	#ifdef WIN32

		#include <winsock2.h> /* Sockets pour Windows */
		typedef int socklen_t;
		
	#elif defined (linux)

		#include <sys/socket.h>
		#include <netdb.h>		/* Host by name */
		#include <linux/types.h>
		#include <sys/ioctl.h>
		#include <net/if.h>

		/* Sous Windows, "winsock2.h" definit certaines constantes qui ne sont pas definies par "socket.h" sous Linux */
		#define INVALID_SOCKET -1
		#define SOCKET_ERROR -1

		/* Macro pour remplacer les 'closesocket' par 'close' sous Linux */
		#define	closesocket(s) close(s)
		
		/* Sous Windows, le type SOCKET (qui, en fait, est un 'int') est obligatoire.
		On va donc definir le type SOCKET sous Linux comme etant un 'int', pour maximiser la portabilite du programme */
		#ifndef SOCKET
			typedef int SOCKET; 
		#endif

		/* Divergences egalement pour le nom des structures (casse) */
		typedef struct sockaddr_in SOCKADDR_IN;
		typedef struct sockaddr	SOCKADDR;
		typedef struct in_addr IN_ADDR;

	#endif

	SOCKET g_sock = INVALID_SOCKET;

int send_message( char* message ) {

	SOCKET sock = g_sock;

	if ( sock == INVALID_SOCKET ) {
		return -2;
	}

	int size = strlen(message);

	if ( send(sock, message, size+1 , 0) < 0 )  {

		printf("Receving error : receive message\n");

		return -1;

	}

	printf("SEND : '%s'\n", message);

	return 0;

}

char* recv_message() {

	SOCKET sock = g_sock;

	if ( sock == INVALID_SOCKET ) {
		return NULL;
	}
	
	char* buffer = malloc( sizeof(char) * 2000 );
	int len = 0;


	if ( ( len = recv(sock, buffer, 2000, 0)) < 0 )  {

		printf("Receving error : receive message\n");

		return NULL;

	}

	char* message = strdup(buffer);

	printf("RECV : %d bytes\n",len);
	printf("RECV : '%s'\n", message);
			
	return message;
}


/* Fonction de depart. Attente des Clients et creation d'un Thread par Client */
int wait_for_client(int port) {

	#ifdef WIN32
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
	#endif


	SOCKADDR_IN local_address;
	SOCKET sock = INVALID_SOCKET;
	socklen_t la_size = sizeof(local_address);

	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ) {
		printf("Error : Unable to listen\n");
		return 3;
	}
	
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);
	local_address.sin_family = AF_INET;
	local_address.sin_port = htons(port);

	if ( (bind(sock, (SOCKADDR*) &local_address, la_size)) == SOCKET_ERROR ) {
		printf("Error : Localhost not found\n");
		return 4;
	}


	printf("Listening on port %d ...\n",port);
	listen( sock, 1 );

	SOCKADDR_IN client_address;
	SOCKET sock_in = INVALID_SOCKET;
	socklen_t ca_size = sizeof(client_address);

	if ( (sock_in = accept(sock, (SOCKADDR*)&client_address, &ca_size)) == INVALID_SOCKET ) {

		printf("Accept error\n");
		return 5;

	}

	printf("Incoming connection (with socket %d of %d:%d\n", sock_in, inet_ntoa(client_address.sin_addr), htons(client_address.sin_port));

	g_sock = sock_in;

	return 0;
		
}


/* Initialise une connexion */
int connect_to_server( const char* server_name, int port ) {

	SOCKET sock = INVALID_SOCKET;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent* hostinfo = NULL;
	SOCKADDR_IN socket_in = { 0 };

	hostinfo = gethostbyname(server_name);

	printf("Connecting to %s:%d ...\n", server_name,port);

	if (hostinfo == NULL) {
		printf("%s : Unknown host\n", server_name);
		return -2;
	}

	socket_in.sin_addr = *(IN_ADDR*) hostinfo->h_addr;
	socket_in.sin_port = htons(port);
	socket_in.sin_family = AF_INET; /* IPv4 TCP */

	if ( connect(sock, (SOCKADDR*) &socket_in, sizeof(socket_in)) == SOCKET_ERROR ) {

		printf("Connection error ! \n");
		return -1;

	}

	printf("Connection established ! \n");

	g_sock = sock;

	return 0;
	
}