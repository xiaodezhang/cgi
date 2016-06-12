#ifndef __FILE_TOOL_H__
#define __FILE_TOOL_H__

#include "cgi.h"


int file_content_get(const char* filename, char* contentBuf);

extern int download_file( const char *filename,  FCGX_Stream * out );
extern int upload_file( const char *filename,  FCGX_Request *pstWebRequest );


#endif

