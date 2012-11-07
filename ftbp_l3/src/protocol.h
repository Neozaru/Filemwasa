#ifndef H_PROTOCOL_H
	#define H_PROTOCOL_H
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "network.h"
	
	enum peer_states {
		CONNECTED,
		DISCONNECTED,
		UNKNOWN
	};
	
	/* Definit les types de "nmessage" 
		pouvant transiter  */
	enum message_type {
		STATUS_MESSAGE = 126,
		CMD_MESSAGE = 129,
		RETURN_MESSAGE = 219
	};
	
	/* Dans le cas d'un "nmessage" de type "CMD_MESSAGE",
		on trouve la liste des commandes ici */
	enum cmds {
		CMD_CONNECT,
		CMD_DISCONNECT,
		CMD_POST_FILE,
		CMD_GET_FILE,
		CMD_CD,
		CMD_LS
	};
	
	/* Dans le cas d'un "nmessage" de type "RETURN_MESSAGE",
	on trouve la liste des statuts ici */
	enum return_status {
		RS_ACCEPTED = 200,
		RS_ERROR_FILE_NOT_FOUND = 404,
		RS_ERROR_FILE_ALREADY_EXISTS = 405,
		RS_ERROR_ACCESS_DENIED = 550,
		RS_ERROR_BAD_FORMAT = 403
	};
	
	/* Un "peer", ici, sera un Client. Cette structure stocke des
		informations pour chaque Client connecte */
	typedef struct peer_s {
		int state;
		SOCKET sock;
		SOCKADDR_IN address;
		char current_directory[256];
	} peer_t;		
			
	/* Definit la maniere dont une liste de fichier sera envoyee */
	typedef struct folder_listing_policy_s {
		int nb_total;		// Total d'elements a envoyer
		int nb_by_data;	// Nombre d'éléments par "flux"
		char path[256];	// Chemin du repertoire liste
	} folder_listing_policy;
	
	
	
	/* Pas certain que cette structure soit utile :
	Sert à caster un "nmessage" de type "CMD_MESSAGE" 
	La structure devient alors plus claire (fonction + argument)
	En fait, on fait juste abstraction du membre "arg1" d'une
	structure "nmessage". */
	typedef struct cmd_t {
		int cmd_type;
		char cmd_arg[256];
	} cmd;
	
	/* Table de correspondance entre une constante et une fonction */
	typedef struct cmd_function_t {
		int cmd_value;
		int (*cmd_fun) ( peer_t* ,char[] );
	} cmd_function;
		
	/* Utilise une table de correspondance "cmd_function" pour retourner
		une fonction d'apres un identifiant ou une constante */
	int 	( *getCmdFunction( int cmdIn, cmd_function fTab[] ) )		( peer_t*, char[] );
	

#endif
