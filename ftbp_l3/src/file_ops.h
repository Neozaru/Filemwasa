#ifndef H_FILEOPS_H
	#define H_FILEOPS_H
	
	
	#include "fm_utils.h"
	#include "file_utils.h"
	// #include "server.h"
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	
	#define DEFAULT_FRAGMENT_SIZE 1340 // octets
	#define MAX_FRAGMENT_SIZE 1492 // octets

	#define DEFAULT_CHUNK_SIZE 1000000 // octets
	
	typedef char path_type[ 256 ];
	typedef char* chunk_data;
	typedef int request_t;
	// typedef unsigned char frag_type[ MAX_FRAGMENT_SIZE ];

	typedef struct file_chunk_t {
		int id;
		chunk_data data;
	} file_chunk;
	
	typedef struct file_splitting_configuration_t {
		int chunk_size;
		int nb_chunks;
		int last_chunk_size;
	} file_splitting_configuration;
	
	typedef struct file_info_t {
		int file_size;
		file_splitting_configuration splitting_configuration;
		md5_value file_checksum;
		path_type file;
	} file_info;
	
	int free_chunk_memory(file_chunk* fc) ;
		
	int fragment_chunk( file_info* fileInfos, file_chunk* chunk_to_fragment) ;
	int assemble_chunk( file_info* fileInfos, file_chunk* chunk_to_assemble) ;
	
	int local_file_test(const char* fileInPath, const char* fileOutPath) ;
	int prepare_file_info( char* filePath, file_info* freq ) ;
	void print_file_info( file_info* fileInfos ) ;
	
	int configure_splitting( file_info* fileInfos ) ;
	
	int abortFileOps();
		
   file_info new_fileInfosuest();
	
	
#endif
