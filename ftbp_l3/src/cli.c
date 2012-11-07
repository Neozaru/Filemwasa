#include "cli.h"

#include "fm_utils.h"
#include "client.h"

/* Choix d'un utilisateur. Renvoie "1" pour "oui", sinon 0 */
int userChoiceCli( char choice[] ) {

	int userChoice = -1;

	while ( userChoice < 0 ) {

		char response[16];
		printf("%s ",choice);
		scanf("%s", response);
	
		if ( response[0] == 'y' || response[0] == 'Y' ) {
			userChoice = 1;
		}
		else if ( response[0] == 'n' || response[0] == 'N' ) {
			userChoice = 0;
		}
		
	}
	
	return userChoice;

}

int userCmdExit( int argc, char** args ) {

	userCmdDisconnect( argc, args);
	exit(0);
	
}


int userCmdDisconnect( int argc, char** args ) {

	if ( argc > 1 ) {

		disconnect_from_server();
	}
	else {
		disconnect_from_server();
	}
	
	return 0;

}


int userCmdConnect ( int argc, char** args ) {

	if ( argc > 1 ) {
		
		int port = DEFAULT_PORT;
		char* server = args[1];
		
		char hostname_without_port[256];
		int port_without_hostname = -1;

		if ( (port_without_hostname = explodeServerPort( (const char*) server, hostname_without_port )) > 0 ) {
			port = port_without_hostname;

			server = strdup(hostname_without_port);
			debugpf("Server : %s -- Port : %d\n", server, port);
		}
		
		
		if ( argc > 2 ) {
			port = atoi( args[2] );
		}
		
		return connect_to_server( server, port );
		
	}
	else {
		return connect_to_server( "localhost", DEFAULT_PORT );
	}

	
	return 0;
	
}

int userCmdSend( int argc, char** args ) {

	if ( argc < 2 || argc > 3 ) {
		help_userCmd( args[0] );
		return -1;
	}
	
	if ( argc == 3 ) {
		sendFile( (const char*) args[1], args[2] );
	}
	else {
		sendFile( (const char*) args[1], "" );
	}

	
	return 0;
}

int userCmdGet( int argc, char** args ) {

	if ( argc < 2 || argc > 3 ) {
		help_userCmd( args[0] );
		return -1;
	}
	
	if ( argc == 3 ) {
		recvFile( (const char*) args[1], args[2] );
	}
	else {
		recvFile( (const char*) args[1], "" );
	}


	return 0;
}

int userCmdHelp( int argc, char** args )  {

	if ( argc > 1 ) {
		help_userCmd( args[1] );
	}
	else {
		print_cmd_list();
	}
	
	return 0;

}

int userCmdLs( int argc, char** args ) {

	if ( argc == 1 ) {
		list_files( "" );
		return 0;
	}
	else if ( argc == 2 ) {

		list_files( args[1] );
		return 0;
		
	}
	else {
		help_userCmd( args[0] );
	}
	
	return -1;


}

int userCmdRm( int argc, char** args ) {

	if ( argc == 2 ) {
		return remove_file( args[1] );
		
	}
	else {
		help_userCmd( args[0] );
	}
	
	return -1;


}

int userCmdCd( int argc, char** args ) {

	if ( argc == 1 ) {
		changeDirectory( "" );
		return 0;
	}
	else if ( argc == 2 ) {

		changeDirectory( args[1] );
		return 0;
		
	}
	else {
		help_userCmd( args[0] );
	}
	
	return -1;

}

int userCmdRcd( int argc, char** args ) {

	if ( argc == 1 ) {
		remoteChangeDirectory( "" );
		return 0;
	}
	else if ( argc == 2 ) {

		remoteChangeDirectory( args[1] );
		return 0;
		
	}
	else {
		help_userCmd( args[0] );
	}
	
	return -1;

}

int userCmdRls( int argc, char** args ) {

	if ( argc == 1 ) {
		remoteListDirectory( "." );
		return 0;
	}
	else if ( argc == 2 ) {

		remoteListDirectory( args[1] );
		return 0;
		
	}
	else {
		help_userCmd( args[0] );
	}
	
	return -1;


}

/* Table de correspondances : Commandes <-> Fonctions 
	Permet egalement de creer facilement des alias, et aides */
static user_cmd userCmdTab[] = {

	{ "connect", userCmdConnect, "Connect to a server", "connect <hostname>[:<port>]", 1 },
	{ "disconnect", userCmdDisconnect, "Disconnect from server", "", 1 },
	{ "delete", NULL, "Delete a file from server", "delete <file>", 0 },										// Inactif (par securite)
	{ "rename", NULL, "Rename a file from server", "rename <file> <new_filename>", 0 }, 	// Inactif (par securite)
	{ "move", NULL, "Move a file from server", "move <file/path> <new_file/path>", 0 },	// Inactif (par securite)
	{ "copy", NULL, "Copy a file from server", "Copy <file/path> <new_file/path>", 0 },		// Inactif (par securite)
	{ "ls", userCmdLs, "List directory files", "ls <path>", 1 },
	{ "rls", userCmdRls, "List directory files on server", "rls <path>", 1 },
	{ "cd", userCmdCd, "Change directory", "cd <path>", 1 },
	{ "rcd", userCmdRcd, "Change directory on server", "rcd <path>", 1 },
	{ "send", userCmdSend, "Send a file to server", "send <source_file> [<target_path>[/<target_source_file>]]", 1 },
	{ "put", userCmdSend, "Send a file to server", "put <source_file> [<target_path>[/<target_source_file>]]", 0 }, // alias
	{ "recv", userCmdGet, "Get a file from server","recv <target_file> [<local_path>[/<target_source_file>]]", 1 },
	{ "get", userCmdGet, "Get a file from server","get <target_file> [<local_path>[/<target_source_file>]]", 0 }, // alias
	{ "help", userCmdHelp, "Print help", "help [cmd]", 1 },
	{ "exit", userCmdExit, "Exit the program", "\t\t\t\t\t", 1 },
	{ "quit", userCmdExit, "Exit the program", "\t\t\t\t\t", 0 }, // alias
	{ NULL, NULL, NULL, NULL }
	
};

int print_cmd_list() {

	int i=0;
	
	while ( userCmdTab[i].cmd_string != NULL ) {
	
		if ( userCmdTab[i].cmdFun != NULL && userCmdTab[i].help == 1 ) {
			printf("%s\t%s\t\t%s\n", userCmdTab[i].cmd_string, userCmdTab[i].cmd_usage, userCmdTab[i].cmd_descr);
		}
		i++;
		
	}
		
	return 0;

}

void help_userCmd( const char* cmdIn ) {

	int i=0;
	
	while ( userCmdTab[i].cmd_string != NULL ) {
	
		if ( strcmp(userCmdTab[i].cmd_string, cmdIn) == 0 ) {
			printf("%s: \n%s\t\tUsage : %s\n", cmdIn, userCmdTab[i].cmd_descr, userCmdTab[i].cmd_usage);
			return 0;
		}
		else {
			i++;
		}
		
	}
	
	printf("%s : Unknown command\n", cmdIn);
	
	return -1;
}

int (* getUserCmdFunction( char* cmdIn )) ( int, char** ) {

	int i=0;
	
	while ( userCmdTab[i].cmd_string != NULL ) {
	
		if ( strcmp(userCmdTab[i].cmd_string, cmdIn) == 0 ) {
			return userCmdTab[i].cmdFun;
		}
		else {
			i++;
		}
		
	}
	
	return -1;
}

void onSigTerm(int sig) {

	disconnect_from_server();
	exit(0);
	
}

int executeCmd( char cmd[] ) {

	char** args = (char**) calloc ( DATA_NB_MAX_ARGS,  MAX_ARG_SIZE * sizeof(char) ); // Nombre maxi d'arguments : 64

	int argc = -1;
	if ( (argc = explode_string_to_args(cmd, args)) > 0 ) {
		
		// debugpf("Cmd : %s\tArgs : %d\n", command, argc);
		
		int (* userCmdFunction)( int, char** );
		if ( (userCmdFunction = getUserCmdFunction( args[0] )) != -1 ) {
			userCmdFunction( argc, args );
		}
		else {
			printf("Incorrect command. Type \"help\" to get a list of available commands");
		}
		
	}
	
}

int cli( int argc, char** args ) {

	// pour envoyuer un paquet de deconnexion en cas de fermeture
	
	#ifdef WIN32
		signal( SIGINT, onSigTerm);
		signal( SIGTERM, onSigTerm);
	#elif defined (linux)
		struct sigaction act;

		act.sa_flags = SA_SIGINFO;
		act.sa_handler = onSigTerm;      /* fonction à lancer */

		sigemptyset(&act.sa_mask);        /* rien à masquer */

		sigaction(SIGINT, &act, NULL);    /* fin contrôle-C */
		sigaction(SIGTERM, &act, NULL);   /* arrêt */
	#endif

	int status = 1;
	
	if ( argc > 0 ) {
		int (* userCmdFunction)( int, char** );
		if ( (userCmdFunction = getUserCmdFunction( args[0] )) != -1 ) {
			userCmdFunction( argc, args );
		}
		else {
			printf("Incorrect command. Type \"help\" to get a list of available commands");
		}
		
		disconnect_from_server();
	}
	else {
	
		while ( status == 1 ) {
	
			printf("\n%s # %s> ", getCurrentDirectory(), PROG_NAME);
			char command[512];
			// fgets(command, MAX_CMD_SIZE,stdin);
			//gets(command);
			if (gets(command) == NULL) {
				printf("Read error. (command line is too long ?)\n");
				abort();
			}

			executeCmd( command );
	
		}
		
	}

	return 0;
}



