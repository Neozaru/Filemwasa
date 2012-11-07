#ifndef H_CLI_H
	#define H_CLI_H
	
	#include <stdio.h>
	#include <string.h>
	
	#include <signal.h>

	
	typedef struct user_cmd_t {
		const char* cmd_string;
		int (* cmdFun) ( int argc, char** args);
		const char* cmd_descr;
		const char* cmd_usage;
		int help; // 1 = will be printed in global help
	} user_cmd;
	

	int cli( int argc, char** args );
	
	#define PROG_NAME "FileMill"
	#define MAX_CMD_SIZE 1024
	#define MAX_ARG_SIZE 256
	
#endif
