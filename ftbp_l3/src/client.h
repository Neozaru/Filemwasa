#ifndef H_CLIENT_H
	#define H_CLIENT_H
	
	#include "network.h"
	
	#include <stdio.h>
	#include <stdlib.h>


		
	#define MAX_DATA_SIZE 1024
	#define DEFAULT_PORT 14441
	#define CLIENT_VERSION 0.1

	enum connection_status {
		STATUS_DISCONNECTED,
		STATUS_CONNECTED
	};
	
	enum launch_modes {
		INTERACTIVE_MODE,
		BASH_MODE
	};
	
	typedef struct client_status_s {
		int connection_status;
		int discovered_mtu; // Not implemented
		SOCKET sock_to_server;

		char server_name[256];
		char current_directory[256];
	} client_status_t;

	
	char* getCurrentDirectory() ;
	
	int changeDirectory( const char* dir ) ;

	int remoteChangeDirectory( const char* dir ) ;

	int list_files( const char* dir ) ;

	int remoteListDirectory( const char* dir ) ;

	int sendFile( const char* filePathIn, const char* filePathOut ) ;

	int recvFile( const char* source_file, const char* dest_file ) ;

	int disconnect_from_server() ;

	int connect_to_server( const char* server_name, int port ) ;
	
#endif
