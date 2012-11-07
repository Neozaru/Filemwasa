#ifndef H_SERVER_H
	#define H_SERVER_H
	

	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>

	
	#include "network.h"
	#include "protocol.h"


	int funPostFileRequest( peer_t* client, char cmd_arg[] ) ;
	int funGetFileRequest( peer_t* client, char cmd_arg[] ) ;
	int funDisconnect( peer_t* client, char cmd_arg[] ) ;
	int funChangeDirectory( peer_t* client, char cmd_arg[] );
	int funDirectoryListing( peer_t* client, char cmd_arg[] );

	int wait_for_cmds( peer_t* client ) ;

	
	cmd_function cmd_tab[] = {
		{ CMD_CONNECT, NULL },
		{ CMD_DISCONNECT, funDisconnect},
		{ CMD_POST_FILE, funPostFileRequest },
		{ CMD_GET_FILE, funGetFileRequest },
		{ CMD_CD, funChangeDirectory },
		{ CMD_LS, funDirectoryListing },
		{ 0, NULL }
	};

	#define MAX_DATA_SIZE 1024
	#define MAX_HOSTNAME_SIZE 128
	#define DEFAULT_PORT 14441
	#define SERVER_VERSION 0.1
	#define NB_MAX_CLIENTS 128
	
	
#endif
