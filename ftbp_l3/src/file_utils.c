#include "file_utils.h"

int rename_file(const char* source, const char* destination) {
   
	printf("Renaming %s to %s ...\n", source, destination);

    return rename( source, destination );

}

int remove_file( const char* filePath ) {

	return	unlink( filePath );

}

char* readFile( FILE* fp ) {


  if( !fp ) {
    return NULL;
  }

  char *output = NULL;
  char buf[ MAX_BUF ];
  int bytes, byteCount = 0;
  
  memset( buf, 0, sizeof( buf ) );
  
	while( ( bytes = fread( buf, sizeof( char ), sizeof( buf ) - 1, fp ) ) > 0 ) {
	
		if( !output ) {
			printf("\nAlloc out %d bytes\n", bytes);
			/* allocate output */
			output = ( char * )malloc( bytes + 1 );
			memcpy( output, buf, bytes + 1 );
		}
		else {
		  
			output = ( char * )realloc( output, byteCount + bytes + 1 );


			memcpy( output + byteCount, buf, bytes + 1 );
		}

		byteCount += bytes;
		memset( buf, 0, sizeof( buf ) );
		
	}
  
  return output;
  
  
}

/* Calcule le hash MD5 d'un fichier */
int MDFile (char* filePath, struct md5_ctx* ctx, unsigned char digest[16]) {

	FILE* f = fopen(filePath, "rb");
	
	if ( f == NULL ) {
		printf("%s file not found.\n", filePath);
		return -2;
	}

	int i;
	unsigned char retStr [33];
	
	md5_init (ctx);
	while (!feof (f)) {
		ctx->size += fread (ctx->buf + ctx->size, 1, MD5_BUFFER - ctx->size, f);
		md5_update (ctx);
	}
	fclose(f);
	md5_final (digest, ctx);

	for (i=0; i<16; i++) {
	
		printf ("%02x", digest [i]);

	
		char buffTemp[3];
		sprintf (buffTemp,"%02x", digest [i]);
		memcpy(&retStr[ i * sizeof(unsigned char) * 2], buffTemp,  sizeof(unsigned char) * 2);
		
	}
	
	retStr[32] = '\0';
	
	printf("R : %s\n", retStr);
	
	
	return 0;
	
}


int getFileSize( char* filePath ) {

	if ( strlen(filePath) > 0 )  {

		FILE* fileIn = NULL;

		fileIn = fopen( filePath, "rb" );

		if ( fileIn != NULL ) {

			fseek(fileIn, 0, SEEK_END);
			int filesize = ftell(fileIn);
			fseek(fileIn, 0, SEEK_SET);
			fclose( fileIn );
			
			return filesize;
		}
	}
	else {
		return -1;
	}

}

char* extract_filename( const char* filePath ) {
	
	char* pathIn = strdup(filePath);
	char* filename = NULL;
	unsigned int pos = 0;
	
	if ( (pos = strrchr( (const char*) pathIn, PATH_DELIM )) != NULL  || (pos = strrchr( (const char*) pathIn, PATH_DELIM2 )) != NULL ) { // A REVOIR
	
		if ( pos - (int) pathIn < strlen(pathIn) ) {
			filename = (char*) ( pos + 1 );
		}
	
	}

	if ( filename && strlen(filename) < 1 ) {
			filename = pathIn;
	}
	
	if ( filename == NULL ) {
		filename = strdup( filePath );
	}
	
	return filename;
	
}

char* extract_path( const char* filePath ) {

	const char* pathIn = filePath;
	char* path = NULL;
	//char* filename = "";
	int pos = 0;
	
	if ( isLocalDirectory( filePath ) == 1 ) {
		path = strdup(pathIn);
	}
	else if ( (pos = strrchr( (const char*) pathIn, PATH_DELIM )) != NULL || (pos = strrchr( (const char*) pathIn, PATH_DELIM2 )) != NULL ) { // A REVOIR
	
			path = strdup(pathIn);
			path[pos-(int) pathIn] = '\0';

	}
	
	return path;

}

void print_md5_digest(	md5_value md5 ) {

	printf("Checksum : ");
	int i;
	for (i=0; i<16; i++) {
		printf ("%02x", md5[i] );
	}
	printf("\nEnd Checksum\n");
	
}

int isLocalDirectory( const char* path ) {

	if ( path != NULL ) {

		DIR* dir = NULL;
		dir = opendir( path );
		
		if ( dir == NULL ) {
			return -2;
		}
		else {
			closedir(dir);
			return 1;
		}
		
	}
	else {
		return -1;	
	}
	
}

int isLocalFile( const char* path ) {

	if ( path != NULL ) {


		FILE* file = NULL;
		file = fopen( path, "r" );
		
		if ( file == NULL ) {
			return -2;
		}
		else {
			debugpf("File : %d\n", file);
			fclose(file);
			return 1;
		}
	}
	else {
		printf("NULL\n");	
		return -1;
	}
	
}

int check_for_destination( const char* filepath ) {
	
		if ( isLocalFile( filepath ) > 0 ) {
			return ERR_EXISTINGFILE;
		}
		else if (  extract_path(filepath)  == NULL || isLocalDirectory( extract_path(filepath) ) ) {
			debugpf("Path : %s", extract_path(filepath) );
			return 1;
		}
		
		return ERR_NOTDIRECTORY;
	
}

struct dirent* ls_dir( char* path ) {


	struct dirent* s_dir;
	DIR* dir = NULL;
	dir = opendir( path );
	
	if ( dir != NULL ) {
	
		while ( (s_dir = readdir( dir ) ) != NULL ) {
			printf("%s\n", s_dir->d_name); 
		}
	
		closedir(dir);
		
		return s_dir;
	}
	
	return NULL;
	
}


