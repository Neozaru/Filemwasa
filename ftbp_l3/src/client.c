#include "client.h"

#include "fm_utils.h"
#include "protocol.h"
#include "file_ops.h"
/*
Client flexible et leger : Communique avec le serveur par texte. Il envoie des chaines de caractere, et en recoit du serveur.
Finalement, seul le serveur définit les règles. Les clients envoient simplement des données par des sockets TCP.
Le troisieme argument du programme envoie des donnees au serveur au moment de la connexion. Suivant les serveurs, ces donnees
peuvent etre utiles ou non. Dans la conception de notre serveur nous adopterons une certaine syntaxe, afin que notre client
puisse, par exemple, envoyer le pseudonyme au moment de la connexion. (ce qui n'exclut pas de pouvoir le definir une fois connecte).

Au moment de la conception du client, nous imaginons déjà plusieurs types de jeu possible : Quizz, pendu, etc ...
Les differents modes de jeu seront finalement definies par le serveur. Le but etant de s'initer au fonctionnement des reseaux, nous n'avons pas estime utile de cree de jeux a interface graphique. Le programme en aurait ete plus lourd, et moins evolutif (ici, on pourra faire evoluer le serveur sans toucher au client).
*/

client_status_t state;

int reset_client_state() {

	state.connection_status = STATUS_DISCONNECTED;
	state.sock_to_server = INVALID_SOCKET;
	strcpy(state.server_name, "");
	char buffer[256];
	strcpy(state.current_directory, getcwd(buffer, 256-1) );
	
	return 0;

}

int userChoice( char choice[] ) {

	// On remonte vers le CLI
	// Cette fonction intermediaire est la pour montrer que, si un GUI
	// était en place, il serait facile d'appeler une pop-up ici.
	return userChoiceCli ( choice );

}

int isConnected() {
	return ( state.connection_status == STATUS_CONNECTED );
}

char* getCurrentDirectory() {
	return state.current_directory;
}

/* Cd local */
int changeDirectory( const char* dir ) {

	if ( strlen(dir) < 256 ) {
		if ( chdir(dir) == 0 ) {
			char buffer[256];
			strcpy(state.current_directory, getcwd(buffer, 256-1) );
		}
		else {
			printf("Cd error\n");
		}
		
		return 0;
	}

	return ERROR_BAD_ARGUMENT_LENGTH;

}

/* Cd distant */
int remoteChangeDirectory( const char* dir ) {

	if ( isConnected() ) {


		 if ( strlen(dir) < 256 ) {
		 
			nmessage cdRequest = { CMD_MESSAGE, CMD_CD, "" };
			strcpy(cdRequest.string, dir);
			if ( send_nmessage( state.sock_to_server, &cdRequest ) == 0 ) {
				
				nmessage cdResponse = { 0, 0, "" };
				if ( recv_nmessage( state.sock_to_server, &cdResponse ) == 0 ) {
				
					if ( cdResponse.arg1 == RETURN_MESSAGE && cdResponse.arg2 == RS_ACCEPTED ) {
					
						printf("Cd ok : %s\n", cdResponse.string);
					
						return 0;
					}
					else {
						printf("Server returned error %d\n", cdResponse.arg2);
						return cdResponse.arg2;
					}
				
				
				
				}
				
				return ERROR_RECV_ERROR;
			}
			
			return ERROR_SEND_ERROR;
		}

		return ERROR_BAD_ARGUMENT_LENGTH;
	
	}
	else {
		printf("Not connected\n");
		return ERROR_CLIENT_NOT_CONNECTED;
	}

}

/* ls local */
int list_files( const char* dir ) {
	
	// const char* completePath = strcat( state.current_directory, dir );
	// ls_dir( completePath );
	
	if ( strlen(dir) == 0 ) {
		ls_dir( state.current_directory );
		return 0;
	}
	else if ( strlen(dir) < 256 ) {
	
		ls_dir( dir );
		
		return 0;
	}

	return ERROR_BAD_ARGUMENT_LENGTH;
	


}

/* ls distant */
int remoteListDirectory( const char* dir ) {

	if ( isConnected() ) {

		 if ( strlen(dir) < 256 ) {
		 
		 	// Demande d'un LS du repertoire
			nmessage lsRequest = { CMD_MESSAGE, CMD_LS, "" };
			
			// Si on a demande le listage d'un autre repertoire que le courant
			strcpy(lsRequest.string, dir);
			if ( send_nmessage( state.sock_to_server, &lsRequest ) == 0 ) {
				
				nmessage lsResponse = { 0, 0, "" };
				if ( recv_nmessage( state.sock_to_server, &lsResponse ) == 0 ) {
				
					if ( lsResponse.arg1 == RETURN_MESSAGE && lsResponse.arg2 == RS_ACCEPTED ) {
						
						dir_list dirlistBase;
						dir_list* dirlist = &dirlistBase; // Pointeur de pointeur, afin de Malloc dans la fonction et de Free en dehors

						if ( recv_dirListing( state.sock_to_server, dir, &dirlist) == 0 ) {
							printf("Grabbed %d elements\n", dirlist->nb_elements);
							
							int i;
							for (i=0; i<dirlist->nb_elements; i++) {
								printf( "%s (%s)\n", dirlist->elements[i].pathname, (dirlist->elements[i].infos & 1<8) == 1 ? "directory" : "file" );
							}
							
							// Grabbed elements SHOULD BE used/copied before next lines (avoid leaks)
							/* Tiens, on avait commence les commentaires en Anglais. Bref, on precise ici que le listage de repertoire 
							est mal exploité, car a la sortie de cette fonction, les données reçues sont supprimées, ce qui implique que 
							tous les traitements doivent s'effectuer ici. */
							
							free(dirlist->elements);
							
							return 0;
						}
						
						free(dirlist->elements);
					
						return -5;
					}
					else {
						printf("Server returned error %d\n", lsResponse.arg2);
						return lsResponse.arg2;
					}
				
				
				
				}
				
				return ERROR_RECV_ERROR;
			}
			
			return ERROR_SEND_ERROR;
		}

		return ERROR_BAD_ARGUMENT_LENGTH;
	}
	else {
		printf("Not connected\n");
		return ERROR_CLIENT_NOT_CONNECTED;
	}
	
	return ERROR_UNKNOWN_ERROR;

}

int sendFile( const char* filePathIn, const char* filePathOut ) {


	/********************************/
	/*			TESTS LOCAUX		*/
	/********************************/

	if ( isLocalFile( filePathIn ) > 0 ) {
	
		char* outFile;
		
		if ( strlen(filePathOut) > 0 ) {
		

			if ( isLocalFile( filePathOut ) ) {
				outFile = strdup( filePathOut );
			}
			else if ( isLocalDirectory( filePathOut ) ) {
			
				char* sTemp = strcat( filePathOut, (const char*) "/");
					
				filePathOut = strcat( sTemp,  extract_filename( filePathIn ) );
			}
			
		}
		else {
			filePathOut =  extract_filename( (const char*) filePathIn );
		}
		
		printf("Outfile : %s -- %s\n", filePathIn, outFile);
		// printf("fn : %s (%s) <%s>\n",outFile,  args[1], extract_path(args[1]));
		
	}
	else {
		printf( "%s file doesn't exists !\n", filePathIn );
		return ERROR_REQUESTED_RESSOURCE_NOT_FOUND;
	}



	/********************************/
	/*		DEBUT DES HOSTILITES	*/
	/********************************/
	
	// Contact the server
	if ( isConnected() ) {
	
		SOCKET sock = state.sock_to_server;
	
		// Envoi de la requete "POST" pour le fichier "filePathOut"
		if ( send_transferRequest( sock, CMD_POST_FILE, filePathOut ) < 0) {
			printf("Errror2\n");
			return ERROR_SEND_ERROR;
		}
		
		/* Checking server status for file */
		nmessage statusMessage = { 0, 0, "" };
		
		if ( recv_nmessage( sock, &statusMessage ) < 0 )  {
			
			printf("Error while receiving response message\n");
			return ERROR_RECV_ERROR;

		}
		
		printf("Status message received (%d)\n", sizeof(statusMessage));

		
		if ( statusMessage.arg2 == RS_ACCEPTED ) {
			
			send_file( sock, filePathIn );
			
		} 
		else {
			printf( "Server returned error %d\n", statusMessage.arg2 );
			return statusMessage.arg2;
		}
		
	}
	else {
		return ERROR_CLIENT_NOT_CONNECTED;
	}

   return 0;
}


int recvFile( const char* source_file, const char* dest_file ) {


	/********************************/
	/*			TESTS LOCAUX		*/
	/********************************/
			
	char* 	outFile =  extract_filename( source_file );

	if ( strlen(dest_file) > 0 ) {
	
		if ( extract_path( dest_file ) != NULL && isLocalDirectory( extract_path( dest_file ) ) ) {
		
			char* sTemp = strcat( extract_path( dest_file ), "/");
							
			outFile = strcat( sTemp,  extract_filename( source_file ) );
		}
		else {
			outFile = strdup( dest_file );
		}
		
	}
	else {
		dest_file = strdup( source_file );	
	}
	
	printf("Outfile : %s\n", outFile);

	/********************************/
	/*		DEBUT DES HOSTILITES	*/
	/********************************/

	if ( isConnected() ) {
	
		SOCKET sock = state.sock_to_server;

		printf( "recv : %s %s \n", source_file, dest_file );

		if ( isLocalFile( dest_file ) <= 0 ) {

			
			// Envoi de la requete "GET" pour le fichier "source_file"
			if ( send_transferRequest( sock, CMD_GET_FILE, source_file ) < 0) {
				printf("Send error\n");
				return ERROR_SEND_ERROR;
			}
			
			/* Checking server status for file */
			nmessage statusMessage = { 0, 0, "" };
			
			if ( recv_nmessage( sock, &statusMessage ) < 0 )  {
				
				printf("Error while receiving response message\n");
				return ERROR_RECV_ERROR;

			}
			
			printf("Status message received (%d)\n", sizeof(statusMessage));

			
			if ( statusMessage.arg2 == RS_ACCEPTED ) {
				
				char* part_target_filename = malloc( ( strlen(dest_file) + 5 + 1 ) * sizeof(char) );
				strcpy( part_target_filename, dest_file );
				strcat( part_target_filename, ".part" );
				
				int already_downloaded_size = 0;
				
				// ICI on peut tester la presence d'un fichier part deja existant
				if ( isLocalFile( part_target_filename ) == 1 ) {
				
					if ( userChoice(".part file found : Do you want to resume a previous download ? [Yes/No]") == 1 ) {
				
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
				
					if ( rename_file( part_target_filename, dest_file ) == 0 ) {
						printf("Download complete !\n");
						return 0;
					}
					else {
						printf("Error while renaming file\n");
					}
				
				}
				
			} 
			else {
				printf( "Server returned error %d\n", statusMessage.arg2 );
				return statusMessage.arg2;
			}
			
			
			close(sock);
		}
		else {
			printf( "%s file already exists !\n", dest_file );
			return ERROR_DESTINATION_RESSOURCE_ALREADY_EXISTS;
		}
	
	}
	else {
		return ERROR_CLIENT_NOT_CONNECTED;
	}

   return 0;
   
}


int disconnect_from_server() {

	abortFileOps();
	// Ferme les fichiers potentiellement en cours de lecture/ecriture

	if ( isConnected() ) {

		SOCKET sock = state.sock_to_server;

		nmessage connectMessage = { CMD_MESSAGE, CMD_DISCONNECT, "Disconnected by user" };
		
		if ( send_nmessage( sock, &connectMessage ) == 0 ) {
			
			printf("Disconnect message sent\n");
			
			closesocket( sock );
			reset_client_state();
		
			return 0;
		}
		
		return ERROR_SEND_ERROR;
		
	}
	else {
		return ERROR_CLIENT_NOT_CONNECTED;
	}

}

/* Initialise une connexion */
int connect_to_server( const char* server_name, int port ) {

	if ( !isConnected() ) {

		SOCKET sock = INVALID_SOCKET;
		sock = socket(AF_INET, SOCK_STREAM, 0);

		struct hostent* hostinfo = NULL;
		SOCKADDR_IN socket_in = { 0 };

		hostinfo = gethostbyname(server_name);


		printf("Connecting to %s:%d ...\n", server_name,port);

		if (hostinfo == NULL) {
			printf("%s : Unknown host\n", server_name);
			return ERROR_CLIENT_UNKNOWN_HOST;
		}

		socket_in.sin_addr = *(IN_ADDR*) hostinfo->h_addr;
		socket_in.sin_port = htons(port);
		socket_in.sin_family = AF_INET; /* IPv4 TCP */

		if ( connect(sock, (SOCKADDR*) &socket_in, sizeof(socket_in)) == SOCKET_ERROR ) {

			printf("Connection error ! \n");
			return ERROR_CLIENT_CONNECT_ERROR;

		}

		nmessage connectMessage = { CMD_MESSAGE, CMD_CONNECT, "" };
		sprintf(connectMessage.string,"%f",CLIENT_VERSION);

		if ( send_nmessage( sock, &connectMessage ) == 0 ) {
			
			printf("Connect message sent\n");
			
			nmessage connectMessageAck = { 0, 0, "" };
			if ( recv_nmessage( sock, &connectMessageAck ) == 0 ) {
				
				if ( connectMessageAck.arg1 == STATUS_MESSAGE && connectMessageAck.arg2 == RS_ACCEPTED ) {
				
					state.connection_status = STATUS_CONNECTED;
					state.sock_to_server = sock;
					strcpy(state.server_name, server_name);
					
					printf("Ack from server\n");
					
					// Non-implemente pour des raisons de portabilite :/
					state.discovered_mtu = getMTU( sock );

					// Connexion etablie
					return 0;
				
				}
			
			}

			return ERROR_RECV_ERROR;
		}
		
		return ERROR_SEND_ERROR;
	}
	else {
		printf( "Already connected to %s\n", state.server_name);
		return ERROR_CLIENT_ALREADY_CONNECTED;
	}

	return ERROR_UNKNOWN_ERROR;
	
}


/*
int main (int argc, char** argv) {

	wsa_init(); // Pour la portabilite windows

	debugpf("Mode debug actif : %d", 1);
	
	reset_client_state();
	int mode = -1;
	int error = 0;

	// 1 parametre (serveur distant) : Mode interactif avec pre-connexion au serveur
	if ( argc > 1 ) {
		
		if ( userCmdConnect(2, argv) != 0 ) {
			error = 1;
		}
		
	}
	
	if ( argc > 2 ) {
	
		mode = BASH_MODE;
		cli( argc-2, &argv[2] );
	
	}
	else {
		mode = INTERACTIVE_MODE;
	}
	
	if ( mode == INTERACTIVE_MODE && error == 0 ) {
		cli( 0, NULL );
	}

	wsa_end();

	return 0;

}
*/

