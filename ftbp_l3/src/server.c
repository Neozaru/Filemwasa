#include "server.h"

#include "fm_utils.h"
#include "file_ops.h"
#include "protocol.h"
/*
Client flexible et leger : Communique avec le serveur par texte. Il envoie des chaines de caractere, et en recoit du serveur.
Finalement, seul le serveur définit les règles. Les clients envoient simplement des données par des sockets TCP.
Le troisieme argument du programme envoie des donnees au serveur au moment de la connexion. Suivant les serveurs, ces donnees
peuvent etre utiles ou non. Dans la conception de notre serveur nous adopterons une certaine syntaxe, afin que notre client
puisse, par exemple, envoyer le pseudonyme au moment de la connexion. (ce qui n'exclut pas de pouvoir le definir une fois connecte).

Au moment de la conception du client, nous imaginons déjà plusieurs types de jeu possible : Quizz, pendu, etc ...
Les differents modes de jeu seront finalement definies par le serveur. Le but etant de s'initer au fonctionnement des reseaux, nous n'avons pas estime utile de cree de jeux a interface graphique. Le programme en aurait ete plus lourd, et moins evolutif (ici, on pourra faire evoluer le serveur sans toucher au client).
*/

peer_t clients[NB_MAX_CLIENTS];


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
	listen( sock, NB_MAX_CLIENTS );


	int nb_clients = 0;

	int i;
	for (i=0; i<NB_MAX_CLIENTS; i++) {

		SOCKADDR_IN client_address;
		SOCKET sock_in = INVALID_SOCKET;
		socklen_t ca_size = sizeof(client_address);
	
		if ( (sock_in = accept(sock, (SOCKADDR*)&client_address, &ca_size)) == INVALID_SOCKET ) {

			printf("Accept error\n");
			return 5;

		}
		// int flags = fcntl(fd, F_GETFL, 0);
		// fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));

		printf("Incoming connection (client number : %d, with socket %d of %d:%d\n", nb_clients+1, sock_in, inet_ntoa(client_address.sin_addr), htons(client_address.sin_port));
			
		
		nmessage connectMessage = { 0, 0, "" };
		if ( recv_nmessage( sock_in, &connectMessage ) == 0 ) {
			
			if ( connectMessage.arg1 == CMD_MESSAGE && connectMessage.arg2 == CMD_CONNECT ) {
			
				if ( atof( connectMessage.string ) <= SERVER_VERSION ) {
				
					nmessage connectMessageAck = { STATUS_MESSAGE, RS_ACCEPTED, "" };
					if ( send_nmessage( sock_in, &connectMessageAck ) == 0 ) {
					
						clients[i].address = client_address;
						clients[i].sock = sock_in;
						clients[i].state = CONNECTED;
						strcpy(clients[i].current_directory, "");
						//pthread_t thread;// On crée un thread
						//pthread_create( &thread, NULL, wait_for_cmds, (void*)&clients[i] );// Permet d'exécuter le fonction maFonction en parallèle

						printf("Connection accepted\n");
						wait_for_cmds(&clients[i]);
					
					}
				
				
				}
			
			}
		
		}
		
	}
	
	close( sock );

	return 0;
}


/* Fonction executee a la reception d'une requete "DISCONNECT" */
int funDisconnect( peer_t* client, char cmd_arg[] ) {

	SOCKET sock = client->sock;

	printf("Client disconnected : %s\n", cmd_arg);
	client->state = DISCONNECTED;
	closesocket( sock );

	return 0;
}

/* Fonction executee a la reception d'une requete "RCD" */
int funChangeDirectory( peer_t* client, char cmd_arg[] ) {

	SOCKET sock = client->sock;
			
	if ( strlen(cmd_arg) < 256 ) {
	
		char newDir[256];
		strcpy( newDir, client->current_directory );
		
		strcat( newDir, cmd_arg );
		format_path( newDir );
		
		if ( isLocalDirectory(newDir) == 1 ) {
			strcpy(client->current_directory, newDir);
		
			nmessage cdResponse = { RETURN_MESSAGE, RS_ACCEPTED, "" };
			strcpy( cdResponse.string, newDir );
			if ( send_nmessage( sock, &cdResponse ) >= 0 ) {

				printf("Directory changed\n");
			
				return 0;
			
			}
		}
		else {
		
			nmessage cdResponse = { STATUS_MESSAGE, RS_ERROR_ACCESS_DENIED, "" };
			if ( send_nmessage( sock, &cdResponse ) >= 0 ) {

				printf("Not a directory (or denied)\n");

				return -1;
			
			}
					
			return -5;
		
		
		}
		
				
		return -3;
		
	}
	else {
		nmessage cdResponse = { STATUS_MESSAGE, RS_ERROR_BAD_FORMAT, "" };
		if ( send_nmessage( sock, &cdResponse ) >= 0 ) {

		
			return -1;
		
		}
				
		return -5;
		
	}
	
	
}

/* Fonction executee a la reception d'une requete "RLS" */
int funDirectoryListing( peer_t* client, char cmd_arg[] ) {

	SOCKET sock = client->sock;

	if ( strlen(cmd_arg) < 256 ) {
	
	
		char newDir[256];
		strcpy( newDir, client->current_directory );
		
		strcat( newDir, cmd_arg );
		format_path( newDir );
		
		if ( isLocalDirectory(newDir) == 1 ) {

			nmessage lsResponse = { RETURN_MESSAGE, RS_ACCEPTED, "" };

			if ( send_nmessage( sock, &lsResponse ) >= 0 ) {

				printf( "Listing dir %s (%s + %s)\n", newDir, client->current_directory, cmd_arg );
			
				send_dirListing( sock, newDir );
				
				printf("Done.\n");
			
				return 0;
			
			}
		}
		
		nmessage lsResponse = { STATUS_MESSAGE, RS_ERROR_BAD_FORMAT, "" };
		if ( send_nmessage( sock, &lsResponse ) >= 0 ) {

		
			return -1;
		
		}
				
		return -3;
		
	}
	else {
	
		nmessage lsResponse = { STATUS_MESSAGE, RS_ERROR_ACCESS_DENIED, "" };
		if ( send_nmessage( sock, &lsResponse ) >= 0 ) {

		
			return -1;
		
		}
				
		return -5;
		
	}
	
}

/* Fonction executee a la reception d'une requete "GET" */
int funGetFileRequest( peer_t* client, char cmd_arg[] ) {

	SOCKET sock = client->sock;

	printf("Enter\n");

	printf("I will post the file %s\n", cmd_arg);
	
	char* fileToSend = malloc ( strlen(cmd_arg) + strlen( client->current_directory ) + 1 );
	strcpy( fileToSend, client->current_directory );
	strcat( fileToSend, cmd_arg );
	
	/* ICI on verifie que le fichier est dispo */
	if ( isLocalFile( fileToSend ) > 0 ) {
	
	
		nmessage messageFileTransferAccepted = { STATUS_MESSAGE, RS_ACCEPTED,  "" };
		strcpy( messageFileTransferAccepted.string, fileToSend );
		
		if ( send_nmessage( sock, &messageFileTransferAccepted ) < 0 )  {
			
			printf("Error while sending nmessage\n");
			return -6;

		}
			
		printf("OK message sent (%d)\n", sizeof(messageFileTransferAccepted));
		
		int ret = send_file( sock, fileToSend );
		if ( ret == -3 ) {
			int a;
		}
		
	}
	else { // If file not found
	
		printf( "%s : Input file not found !\n", fileToSend );
	
		nmessage messageFileNotFound = { STATUS_MESSAGE, RS_ERROR_FILE_NOT_FOUND,  "" };
		strcpy( messageFileNotFound.string, fileToSend );
		
		if ( send_nmessage( sock, &messageFileNotFound ) < 0 )  {
			
			printf("Error while sending nmessage\n");
			return -6;

		}
		else {
			printf("Error message sent (%d)\n", sizeof(messageFileNotFound));
		}

		return -3;
	}
	

	
	return 0;
}

/* Fonction executee a la reception d'une requete "PUT" */
int funPostFileRequest( peer_t* client, char cmd_arg[] ) {

	SOCKET sock = client->sock;

	printf("I will get the file %s\n", cmd_arg);

	char* fileToRecv = malloc ( strlen(cmd_arg) + strlen( client->current_directory ) + 1 );
	strcpy( fileToRecv, client->current_directory );
	strcat( fileToRecv, cmd_arg );
	
	/* ICI on vérifie que le fichier peut bien etre recu */
	if ( isLocalFile( fileToRecv ) <= 0 ) {
	
	
		nmessage messageFileTransferAccepted = { STATUS_MESSAGE, RS_ACCEPTED,  "" };
		strcpy( messageFileTransferAccepted.string, fileToRecv );
		
		if ( send_nmessage( sock, &messageFileTransferAccepted ) < 0 )  {
			
			printf("Error while sending nmessage\n");
			return -6;

		}
			
		printf("OK message sent (%d)\n", sizeof(messageFileTransferAccepted));
		
		char* part_target_filename = (char*) malloc( (strlen(fileToRecv) + 1 + 5) * sizeof(char) );
		sprintf(part_target_filename, "%s.part" , fileToRecv);
		
		int already_downloaded_size = 0;
				
		// ICI on peut tester la presence d'un fichier part deja existant
		if ( isLocalFile( part_target_filename ) == 1 ) {
		
			if ( 1 ) {
		
				already_downloaded_size = getFileSize( part_target_filename );
				printf(".part file found. Resuming previous download...\n");
				
			}
			else {
				unlink( part_target_filename );
				// On detruit le fichier.
			}
		
		}
		// Fin code detection telechargement precedent
		
		if ( receive_file( sock, part_target_filename, already_downloaded_size ) == 0 ) {
		
			printf("Rename %s to %s\n", part_target_filename, fileToRecv );
			if ( rename_file( part_target_filename, fileToRecv ) == 0 ) {
				printf("Download complete !\n");
				return 0;
			}
			else {
				printf("File corrupted\n");
			}
			
		}

		
	}
	else { // If file already exists
	
		printf( "%s : Input file already exists !\n", fileToRecv );
	
		nmessage messageFileAlreadyExists = { STATUS_MESSAGE, RS_ERROR_FILE_ALREADY_EXISTS,  "" };
		strcpy( messageFileAlreadyExists.string, fileToRecv );
		
		if ( send_nmessage( sock, &messageFileAlreadyExists ) < 0 )  {
			
			printf("Error while sending nmessage\n");
			return -6;

		}
		else {
			printf("Error message sent (%d)\n", sizeof(messageFileAlreadyExists));
		}

		return -3;
	}
	
	

	return 0;
}

int wait_for_cmds( peer_t* client ) {

	SOCKET sock = client->sock;
	char current_directory[256];
	
	
	printf( "Connection established for socket %d\n", sock );
	int status = 1;
	int error_count = 0;
	
	while ( status == 1 && client->state == CONNECTED ) {
	
		nmessage message = { 0, 0, "" };
		
		printf("Waiting for command ...\n");
		if ( recv_nmessage( sock, &message ) < 0 ) {
			printf("Client disconnected\n");
			//pthread_exit( -2 );
			return -2;
		}
		
		printf("Message !\n");	
		
		if ( message.arg1 == CMD_MESSAGE ) {
			error_count = 0;
		
			printf("Request. %s\n",message.string);
		
			cmd* command = (cmd*) ( &(message.arg2) );
			
			int (*cmdFunction) ( peer_t*, char[] );
			cmdFunction = getCmdFunction( command->cmd_type, cmd_tab );

			if ( cmdFunction != -1 ) { // A REVOIR

				cmdFunction( client, command->cmd_arg );
			}
		
		}
		else {
			error_count++;
			if (error_count > 4) {
				status = -1;
				printf("Connection lost for unknown reason\n");
			}
		}
		
		
	}
	
	if ( sock != INVALID_SOCKET ) {
		closesocket(sock);
	}
	
	return 0;

}

/*

int main (int argc, char** argv) {

	wsa_init();


	if ( argc != 1 && argc != 2 ) {
		char* help = (char*) malloc(sizeof(char) * 513);
		sprintf(help,"Usage : %s [<listen_port>]\n" \
			"(default port is %d)\n" \
			"Ex : %s 5674\n"
			,argv[0],DEFAULT_PORT,argv[0]);
		printf("%s\n",help);
		exit(1);
	}


	int listen_port = DEFAULT_PORT;

	if (argc == 2) {
		listen_port = atoi(argv[1]);
	}

	wait_for_client( listen_port );

	wsa_end();

	printf("Finished.\n");

	return 0;

}
*/