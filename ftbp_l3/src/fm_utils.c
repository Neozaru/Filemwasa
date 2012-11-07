#include "fm_utils.h"

void debugpf(char* toPrint, ...) {

	#ifdef NQ_DEBUG_MESSAGES

		va_list args;						// Liste d'arguments contenus dans "..." passe en entree
		va_start (args, toPrint);		// On indique au programme que cette liste doit commencer APRES le string "toPrint" (qui, ici, est le format)

		printf("Dbg#> ");
		vprintf(toPrint, args);			// Identique a la fonction printf, mais les [1,n] arguments sont passes via une variable "va_list".
		printf("\n");

		va_end(args);


	#endif

}

void sleep_millis(int millis) {
	#ifdef WIN32
		Sleep(millis);
	#elif defined (linux)
		usleep(millis*1000);
	#endif
		
}

/* Permet de modifier la forme d'un chemin/path
	vers un format type "monChemin/monDossier/"
*/
int format_path( char path[] ) {

	int len = strlen(path);
	// printf("%s\n", path );

	
	if ( len > 0 && len < 256 ) { //255
		// int sp[256] = { -99 };
		char strOut[256] = "";
		int posLastSlash = -99;
		int i;
		int j=0;
		
		for ( i=0; i<len; i++ ) {
			
			if ( path[i] == '/' ) {
			
				// printf("S%d\n", posLastSlash);
				if ( posLastSlash != i-1 ) {
					
					// printf("-");
					strOut[j] = path[i];
					j++;
				}
			
				posLastSlash = i;

			}
			else {
				strOut[j] = path[i];
				j++;
			}
			
		}
		
		if ( strOut[j-1] != '/' ) {
			strOut[j] = '/';
			strOut[j+1] = '\0';
		}
		
		strcpy( path, (const char*) strOut );
		// printf("%s -- %s\n", path, strOut);
		
	}
	else {
		return -1;
	}
	
	return 0;

}

/* Parse une chaine de type "serveur:port" */
int explodeServerPort( const char* arg, char strRet[] ) {

	char* strTmp = malloc( (strlen(arg) + 1) * sizeof(char) );
	strcpy( strTmp, arg );
	
	char delims[] = "/:";
	char* argTemp = NULL;


	argTemp = strtok( strTmp, delims );

	if ( argTemp != NULL ) {

		strcpy( strRet, argTemp );
		
		if ( (argTemp = strtok( NULL, delims ) ) != NULL ) {

			int port = atoi(argTemp);

			free(strTmp);
			return port;
		}

	}
	
	free(strTmp);
	return -1;

}

/* Convertit une chaine entiere avec espaces en arguments multiples */
int explode_string_to_args( char* strIn, char** argsOut ) {

	char* str = strdup(strIn);

	char delims[] = " ";
	char* argTemp = NULL;

	int argCounter = 0;

	argTemp = strtok( str, delims );

	while ( argTemp != NULL && argCounter < DATA_NB_MAX_ARGS ) {

		argsOut[argCounter++] = strdup(argTemp);
	
		argTemp = strtok( NULL, delims );

	}
	
	if ( argTemp != NULL ) {
		free(argTemp);
	}
	free(str);

	return argCounter;

}

/* Pour initaliser WSA, dans Windows */
void wsa_init()
{
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

/* Pour clore WSA, dans Windows */
void wsa_end()
{
#ifdef WIN32
    WSACleanup();
#endif
}


/** DEBUG **/
void print_all_args(int argc, char** args) {

	debugpf("\nListing args (%d) : \n", argc);

	if ( argc > 0 && args[0] != NULL ) {

		int i;
		for (i=0; i<argc; i++) {
			debugpf( "%d : %s\n", i, args[i]) ;
		}

	}
	else {
		debugpf("No args\n");
	}

}
