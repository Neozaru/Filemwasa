
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>

	#include <signal.h>

	#include <sys/queue.h>

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


typedef struct Msg_Queue {
    /* ici on met nos elements par exemple : */
    char* str;

    /*
     * On déclare notre liste avec SLIST_ENTRY qui est equivalente à
     * struct { struct MaStruct *sle_next; } next;
     */

    TAILQ_ENTRY(Msg_Queue) next;
} Msg_Queue;

TAILQ_HEAD(, Msg_Queue) g_head;

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

	return 0;

}

char* g_buffer = NULL;
int available_data = 0;


char* recv_message() {

	/* Si le buffer de reception de message est indisponible, on cherche dans la pile TCP */
	if ( available_data <= 1 ) {

		SOCKET sock = g_sock;

		if ( sock == INVALID_SOCKET ) {
			return NULL;
		}
		
		g_buffer = malloc( sizeof(char) * 2000 );

		if ( ( available_data = recv(sock, g_buffer, 2000, 0)) < 0 )  {

			printf("Receving error : receive message\n");

			return NULL;

		}

	}

	/*  Ce bloc peut se traduire par "Recupere UN message parmi les donnees presentes dans la file" */
	char* message = strdup(g_buffer);
	int consumed = strlen(message)+1;
	available_data -= consumed;
	g_buffer = (g_buffer + consumed);
	/****/


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

	g_sock = sock;


	return 0;
	
}


void exit_program( int code ) {

	if ( g_sock != INVALID_SOCKET ) {
		closesocket(g_sock);
	}

	printf("Exiting program... \n");
	exit(code);
}

void onSigTerm( int sig ) {
	exit_program(9);
}

#if defined (linux)

	struct sigaction act;

#endif

void init_network() {

	// pour envoyuer un paquet de deconnexion en cas de fermeture
	
	#ifdef WIN32
		signal( SIGINT, onSigTerm);
		signal( SIGTERM, onSigTerm);
	#elif defined (linux)

		act.sa_flags = SA_SIGINFO;
		act.sa_handler = onSigTerm;      /* fonction à lancer */

		sigemptyset(&act.sa_mask);        /* rien à masquer */

		sigaction(SIGINT, &act, NULL);    /* fin contrôle-C */
		sigaction(SIGTERM, &act, NULL);   /* arrêt */
	#endif

}


