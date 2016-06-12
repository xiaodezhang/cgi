#include "file_tool.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_FILE_NAME  "copy.log"
#define CONFIG_FILE_NAME  "copy.cfg"


char recv_buf[ 4096 ] = { 0 };


int download_file( const char *filename,  FCGX_Stream * out )
{
	static char     buff[ 8192 ] = { 0 };
	int      n  = 0;  
	time_t   tp = 0; 
	FILE    *fp = NULL;
	struct   stat s = { 0 };
	int      file_name_len = 0;

	if( !filename  || !out )
	{
		return ERR;
	}

	if( access( filename, R_OK ) < 0 )
	{
		return ERR;
	}
	
	tp=time(NULL);
	stat(filename, &s);

	file_name_len = strlen( filename );
	
	FCGX_FPrintF( out, "HTTP/1.0 200 OK\r\nServer: lion\r\nMime-Version:1.0\r\nDate: %s\r\n" ,ctime(&tp));
	FCGX_FPrintF( out, "Content-Type: application/octet-stream/nContent-Length: %ld\r\n" , s.st_size);

	if( !strcasecmp( &( filename[ file_name_len - 4 ] ),  ".log" ) )
	{
		FCGX_FPrintF( out, "Content-Disposition: attachment; filename=\"%s\"\r\n" ,  LOG_FILE_NAME);
	}
	else if ( !strcasecmp( &( filename[ file_name_len - 4 ] ),  ".cfg" ) )
	{
		FCGX_FPrintF( out, "Content-Disposition: attachment; filename=\"%s\"\r\n" ,  CONFIG_FILE_NAME);
	}
	else{
		FCGX_FPrintF( out, "Content-Disposition: attachment; filename=\"%s\"\r\n" ,  "copy.dat" );
	}

	FCGX_FPrintF( out, "Connection: close\r\n\r\n" );
	
	fp=fopen(filename, "r" );	
	memset( buff, 0, sizeof( buff ) );
	while (( n = fread( &buff, sizeof ( char ), sizeof( buff ), fp) ) > 0 ) {
		//FCGX_FPrintF( out, "%s", buff );
		FCGX_PutStr( buff, n, out );
		memset( buff, 0, sizeof( buff ) );
	}
	fclose(fp);

	FCGX_FFlush( out );
	

	return OK;
}

int upload_file( const char *filename,  FCGX_Request *pstWebRequest )
{
	char *content_type     = NULL;
	char *boundary         = NULL;	
	int   boundary_len     = 0;
	char *cur              = NULL;
	int   content_len      = 0;
	int   head_len         = 0;
	int   data_len         = 0;
	int   left_data_len    = 0;
	int   len              = 0;

	FILE  *uploadFile      =  NULL;
	//FILE  *uploadLOG       =  NULL;

	if( !filename || !pstWebRequest )
	{
		return ERR;
	}

	//uploadLOG = fopen( "/data/file_upload.log", "w+" );
	

	/* get upload content length */
	content_len = atoi( FCGX_GetParam( "HTTP_CONTENT_LENGTH", pstWebRequest->envp ) );
	content_type = FCGX_GetParam( "CONTENT_TYPE", pstWebRequest->envp );
	if( !content_type )
	{
		return ERR;
	}

	boundary = strstr( content_type, "boundary=" ) +  strlen( "boundary=" );
	boundary_len = strlen( boundary );	

	//fprintf( uploadLOG, "content_len = %d\n", content_len );
	//fprintf( uploadLOG, "boundary = %s\n", boundary );
	//fprintf( uploadLOG, "boundary_len = %d\n", boundary_len );
	//fflush( uploadLOG );

	uploadFile = fopen( filename, "w+" );
	if( !uploadFile )
	{
		//FCGX_FPrintF( pstWebRequest->out, "create tmp file for uploaded file failure\n" );
		return ERR;
	}

	memset( recv_buf, 0, sizeof( recv_buf ) );
	len = FCGX_GetStr( recv_buf, sizeof( recv_buf ), pstWebRequest->in );

	cur = strstr( recv_buf, "Content-Type:" ) ;
	if( cur ) {
		cur = strstr( cur, "\r\n" ) + 4;
	}
	else
	{
		return ERR;
	}
	
	head_len = cur - recv_buf;
	data_len = content_len - head_len - boundary_len - 8;
	left_data_len = data_len;

	//fprintf( uploadLOG, "head_len: %d; data_len = %d\n", head_len, data_len );
	//fflush( uploadLOG );
	
	//fwrite( recv_buf, 1, head_len, uploadLOG );
	//fflush( uploadLOG );

	

	if( ( len - head_len ) >= data_len )
	{
		fwrite( cur, 1, data_len, uploadFile );
		fclose( uploadFile );
		return OK;
	}
	else
	{	
		fwrite( cur, 1, ( len - head_len ), uploadFile );
		left_data_len -= ( len - head_len );
	}

	memset( recv_buf, 0, sizeof( recv_buf ) );
	
	while( ( len = FCGX_GetStr( recv_buf, sizeof( recv_buf ), pstWebRequest->in ) ) > 0 )
	{
		if( left_data_len > len )
		{
			fwrite( recv_buf, 1, len, uploadFile );
			left_data_len -= len;
		}
		else if( left_data_len <= len )
		{
			fwrite( recv_buf, 1, left_data_len, uploadFile );
			left_data_len = 0;
		}
		
		memset( recv_buf, 0, sizeof( recv_buf ) );
	}

    fclose( uploadFile );

	return OK;
}
