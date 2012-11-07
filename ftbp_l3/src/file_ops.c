#include "file_ops.h"

FILE* fileOperation = NULL; 
/* Si le Client est interrompu, on tente  un "fclose" sur cette variable qui est mise a jour 
en fonction des traitements sur les fichiers. Cela evite que le programme soit interrompu
et que des fichiers restent verrouilles */

/* Lit un Bloc depuis un fichier */
int fragment_chunk( file_info* fileInfos, file_chunk* chunk_to_fragment ) {

	int chunk = chunk_to_fragment->id;

	fileOperation = NULL;
	fileOperation = fopen( fileInfos->file, "rb" );
	
	int frag_counter = 0;

	if ( fileOperation != NULL ) {

		int offset = fileInfos->splitting_configuration.chunk_size * chunk;

		fseek( fileOperation, offset, SEEK_SET);

		int size_count = -1;

		int chunk_size;
		if ( chunk + 1 == fileInfos->splitting_configuration.nb_chunks ) {
		
			chunk_size = fileInfos->splitting_configuration.last_chunk_size;
			printf("read Last chunk !\n");
			
		}
		else {
		
			chunk_size = fileInfos->splitting_configuration.chunk_size;
			
		}

		// frag_type dataBuffer[ chunk_to_fragment->nb_fragments ];
		chunk_to_fragment->data = (char*) malloc ( chunk_size );
		memset(chunk_to_fragment->data, 0, chunk_size);
		size_count = fread (chunk_to_fragment->data, 1, chunk_size, fileOperation);

		printf("c%d read : size is %d\n", chunk, size_count);

		if ( size_count > 0 ) {


		}
		else {
			debugpf("EOF (%d)\n", size_count);

		}

		fclose(fileOperation);
		

		return frag_counter;

	}
	else {
		printf("%s : File not found !\n", fileInfos->file);

		return -1;
	}


}

/* Empile un Bloc dans un fichier. */
int assemble_chunk( file_info* fileInfos, file_chunk* chunk_to_assemble ) {

	int chunk = chunk_to_assemble->id;

	fileOperation = NULL;
	fileOperation = fopen( fileInfos->file, "ab+" );

	int chunk_size;
	if ( chunk+1 == fileInfos->splitting_configuration.nb_chunks ) {

		chunk_size = fileInfos->splitting_configuration.last_chunk_size;
		printf("write Last chunk ! (%d)\n",  chunk+1);

	}
	else {

		chunk_size = fileInfos->splitting_configuration.chunk_size;
		printf("write chunk %d\n", chunk+1);

	}


	fwrite (chunk_to_assemble->data, 1, chunk_size, fileOperation);
	//free(chunk_to_assemble);


	fclose(fileOperation);


	printf("File written\n");

	/*
	free(chunk_to_assemble->fragment);
	free(chunk_to_assemble);
	free(dataBuffer);
	*/

	return 0;
}

/* Empile un Bloc dans un fichier. Methode "propre" et lente.
	(le fichier a ensuite une taille multiple de n Blocs) */
int assemble_chunk_safe( file_info* fileInfos, file_chunk* chunk_to_assemble ) {

	int chunk = chunk_to_assemble->id;

	fileOperation = NULL;
	fileOperation = fopen( fileInfos->file, "rb+" );

	int offset = fileInfos->splitting_configuration.chunk_size * chunk;
	fseek( fileOperation, offset, SEEK_SET);


	int chunk_size;
	if ( chunk+1 == fileInfos->splitting_configuration.nb_chunks ) {

		chunk_size = fileInfos->splitting_configuration.last_chunk_size;
		printf("write Last chunk ! (%d)\n",  chunk+1);

	}
	else {

		chunk_size = fileInfos->splitting_configuration.chunk_size;
		printf("write chunk %d\n", chunk+1);

	}


	fwrite (chunk_to_assemble->data, 1, chunk_size, fileOperation);
	//free(chunk_to_assemble);


	fclose(fileOperation);


	printf("File written\n");

	/*
	free(chunk_to_assemble->fragment);
	free(chunk_to_assemble);
	free(dataBuffer);
	*/

	return 0;
}

void print_file_info( file_info* fileInfos ) {
	printf ("File size : %d\n", fileInfos->file_size );
	printf("\tChunk size : %d\n\tNb_chunks : %d\n\tLast Chunk Size : %d\n"
			, fileInfos->splitting_configuration.chunk_size
			, fileInfos->splitting_configuration.nb_chunks
			, fileInfos->splitting_configuration.last_chunk_size
			);

	print_md5_digest(	fileInfos->file_checksum );

	if ( strlen( fileInfos->file ) > 0 ) {
		printf("source_file (%d) : \n", strlen( fileInfos->file ) );
		printf("\t\t%s\n\n", fileInfos->file);
	}


}

/* Calcul combien de "Blocs" un fichier partiellement telecharge possede deja */
int getChunkOffsetFromSize( int already_downloaded_size, file_splitting_configuration* split_conf ) {

	int already_downloaded_chunks = already_downloaded_size / split_conf->chunk_size;
	printf(" = %d / %d\n", already_downloaded_size, split_conf->chunk_size);
	
	if ( already_downloaded_chunks > 0 ) {
		return already_downloaded_chunks-1;
	}
	else {
		return 0; 
	}

}

/* Genere une structure contenant une politique de decoupage, en fonction des informations fournies en entree */
int configure_splitting( file_info* fileInfos ) {

	if ( strlen(fileInfos->file) > 0 )  {

		int filesize = -1;

		if ( (filesize = getFileSize(fileInfos->file) ) >= 0 ) {
			
			debugpf("File size for '%s' : %d\n", fileInfos->file, filesize);

			int chunk_size =  DEFAULT_CHUNK_SIZE;
			int nb_chunks = filesize / chunk_size;
			int last_chunk_size =  filesize % chunk_size;

			printf("Calculated : %d of %d, and one of %d\n", nb_chunks, chunk_size, last_chunk_size);

			fileInfos->file_size = filesize;
			file_splitting_configuration sp_config = { chunk_size, (nb_chunks+1), last_chunk_size };
			fileInfos->splitting_configuration = sp_config;

		}
		else {

			printf("%s : File not found !\n", fileInfos->file);
			return -2;

		}


	}
	else {
		debugpf("Error : source_file is not initialized\n");
	}

	return 0;

}

	
int prepare_file_info( char* filePath, file_info* freq ) {
	
	strcpy(freq->file,filePath);

	if ( configure_splitting( freq ) == 0 ) {


		printf("File preparation successful\n");
		printf("(o) Calculated : %d of %d, and one of %d\n", freq->splitting_configuration.nb_chunks-1, freq->splitting_configuration.chunk_size, freq->splitting_configuration.last_chunk_size);
		printf("Size of sutruct : %d\n", sizeof(*freq));


		struct md5_ctx md5;
		printf("Detecting md5 ...\n");

		unsigned char digest[16];
		MDFile ( (char*) filePath, &md5, digest );
		memcpy(freq->file_checksum, digest,16);
		
		return 0;

	}
	else {
		printf("File preparation failed\n");
	}

	return -1;

}

file_info new_fileInfosuest() {
   file_info new_fr = { 0, 0,
		                     { 0, 0, 0 }
		                                , "", "", "" };

	return new_fr;
}

int abortFileOps() {

	int res = 1;
	
	#ifdef WIN32
	if ( res != NULL ) {
		res = fclose(fileOperation);
	}
	#elif defined (linux)
		fcloseall();
	#endif
	
	return res;

}
