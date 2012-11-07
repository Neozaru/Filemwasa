#ifndef H_NQUTILS_H
	#define H_NQUTILS_H
	
	enum client_errors_e {
	
		ERROR_CLIENT_NOT_CONNECTED = 1,
		ERROR_CLIENT_ALREADY_CONNECTED,
		ERROR_CLIENT_CONNECT_ERROR,
		ERROR_CLIENT_UNKNOWN_HOST,
		ERROR_BAD_ARGUMENT_LENGTH,
		ERROR_CONNECTION_TIMED_OUT,
		ERROR_SEND_ERROR,
		ERROR_RECV_ERROR,
		ERROR_UNKNOWN_ERROR,
		ERROR_REQUESTED_RESSOURCE_NOT_FOUND,
		ERROR_DESTINATION_RESSOURCE_ALREADY_EXISTS,
		ERROR_CHUNK_QUERY_FLOOD_DETECTED,
		ERROR_BAD_CHUNK_QUERY,
		ERROR_CHUNK_ACK
	
	};
	
	#ifdef WIN32
		#include <winsock2.h>
	#endif	
		
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>
	#include <time.h>
	
	void debugpf(char* toPrint, ...);
	
	int explode_string_to_args( char* strIn, char** argsOut ) ;	
	int explodeServerPort( const char* arg, char strRet[] ) ;
	
	void print_all_args(int argc, char** args) ;
	
	int format_path( char strOut[] );
	
	void wsa_init();
	void wsa_end();
	
	void sleep_millis(int millis);
	
	
	#define DATA_NB_MAX_ARGS 64

#endif
