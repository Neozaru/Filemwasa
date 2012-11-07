#include "protocol.h"


int (* getCmdFunction(int cmdIn, cmd_function fTab[]) )( peer_t* client, char[]) {

	int i=0;
	
	while ( fTab[i].cmd_value > -1 ) {
	
		if ( fTab[i].cmd_value == cmdIn ) {
			return fTab[i].cmd_fun;
		}
		else {
			i++;
		}
		
	}
	
	return -1;

}
