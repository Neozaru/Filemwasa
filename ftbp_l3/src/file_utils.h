#ifndef H_FILE_UTILS_H
	#define H_FILE_UTILS_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include <errno.h>
	#include <ctype.h>
	
	#include <sys/types.h>

	#include <unistd.h>



	#include <fcntl.h> 

	#include "../include/md5.h"

	//buffer maximum
	#define MAX_BUF 1024
	
	#define PATH_DELIM '/'
	
	#ifdef WIN32
		#define PATH_DELIM2  '\\'
	#elif defined (linux)
		#define PATH_DELIM2 '/'
	#endif
	
	#include <dirent.h>
	
	#include "fm_utils.h"
	
	#define ERR_EXISTINGFILE -1
	#define ERR_NOTDIRECTORY -2
	
	typedef struct folder_elt_s {
		char infos;
		int lastModified;
		char pathname[256];
	} folder_elt;
	
	typedef struct dir_list_s {
		int nb_elements;
		folder_elt* elements;
	} dir_list;


	typedef unsigned char md5_value[16];

	char* readFile( FILE* fp);
	// char* strCopy( char* str );
	// bool hash_md5_file(const char* file, char* result);
	int MDFile (char* filePath, struct md5_ctx *ctx, unsigned char digest[16]);
	void print_md5_digest( md5_value md5) ;
	char* extract_filename( const char* filePath ) ;
	int rename_file(const char* source, const char* destination) ;
	
	int isLocalDirectory( const char* path );
	int isLocalFile( const char* path );

	

#endif
