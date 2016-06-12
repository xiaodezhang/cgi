#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
#include "zlib.h"
#include "logger.h"

const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int find_pid_by_name( char *ProcName, int *foundpid )
{
    DIR *dir;
    struct dirent *d;
    int pid,
        i;
    char *s;
    int pnlen;
    char exe[PATH_MAX + 1] = { 0 };
    char path[PATH_MAX + 1] = { 0 };
    int len = 0;
    int namelen = 0;
    int fd = -1;
    
    i = 0;
    foundpid[0] = 0;
    pnlen = strlen( ProcName );
    
    /* Open the /proc directory. */
    dir = opendir( "/proc" );
    if ( !dir )
    {
        return -1;
    }
    
    /* Walk through the directory. */
    while ( ( d = readdir( dir ) ) != NULL )
    {
    
        memset( exe, 0, sizeof( exe ) );
        memset( path, 0, sizeof( path ) );
        len = 0;
        namelen = 0;
        
        /* See if this is a process */
        if ( ( pid = atoi( d->d_name ) ) == 0 )
        {
            continue;
        }
        
        snprintf( exe, sizeof( exe ), "/proc/%s/cmdline", d->d_name );
        fd = open( exe, O_RDONLY );
        if ( fd < 0 )
        {
            closedir( dir );
            return -1;
        }
        
        len = read( fd, path, sizeof( path ) );
        if ( len < 0 )
        {
            close( fd );
            closedir( dir );
            return -1;
        }
        
        close( fd );
        
        /* Find ProcName */
        s = strstr( path, ProcName );
        if ( s == NULL )
        {
            continue;
        }
        
        /* we don't need small name len */
        namelen = strlen( s );
        if ( namelen < pnlen )
        {
            continue;
        }
        
        if ( !strncmp( ProcName, s, pnlen ) )
        {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if ( s[pnlen] == ' ' || s[pnlen] == '\0' )
            {
                foundpid[i] = pid;
                i++;
            }
        }
    }
    
    foundpid[i] = 0;
    closedir( dir );
    
    return 0;
    
}

void write_pin( int fd, int high )
{
    char data = high + '0';
    if ( fd < 0 )
    {
        return;
    }
    lseek( fd, 0, SEEK_SET );
    write( fd, &data, 1 );
    lseek( fd, 0, SEEK_SET );
}

int check_nic( char *nic_name )
{
    struct ifreq ifr;
    int skfd = socket( AF_INET, SOCK_DGRAM, 0 );
    
    strcpy( ifr.ifr_name, nic_name );
    if ( ioctl( skfd, SIOCGIFFLAGS, &ifr ) < 0 )
    {
        close( skfd );
        return false;
    }
    
    close( skfd );

#if 0
    if ( ifr.ifr_flags & IFF_UP )
    {
      	//loge( "%s: IFF_UP\n", nic_name );
    }
#endif    
    
    if ( ifr.ifr_flags & IFF_RUNNING )
    {
    	//loge( "%s: IFF_RUNNING\n", nic_name );
        return true;
    }
    
    return false;
}

char *base64_encode( const unsigned char *bindata, char *base64, int binlength )
{
    int i, j;
    unsigned char current;
    
    for ( i = 0, j = 0; i < binlength; i += 3 )
    {
        current = ( bindata[i] >> 2 );
        current &= ( unsigned char ) 0x3F;
        base64[j++] = base64char[( int ) current];
        
        current = ( ( unsigned char ) ( bindata[i] << 4 ) ) & ( ( unsigned char ) 0x30 );
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[( int ) current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( ( unsigned char ) ( bindata[i + 1] >> 4 ) ) & ( ( unsigned char ) 0x0F );
        base64[j++] = base64char[( int ) current];
        
        current = ( ( unsigned char ) ( bindata[i + 1] << 2 ) ) & ( ( unsigned char ) 0x3C );
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[( int ) current];
            base64[j++] = '=';
            break;
        }
        current |= ( ( unsigned char ) ( bindata[i + 2] >> 6 ) ) & ( ( unsigned char ) 0x03 );
        base64[j++] = base64char[( int ) current];
        
        current = ( ( unsigned char ) bindata[i + 2] ) & ( ( unsigned char ) 0x3F );
        base64[j++] = base64char[( int ) current];
    }
    base64[j] = '\0';
    return base64;
}

int zccompress( const char *data, const int len, char *out, unsigned long *outlen )
{
    z_stream myzlibstream;
    int curroutlen = *outlen;
    int ret = 0;
    myzlibstream.next_in = ( Bytef * ) data;
    myzlibstream.avail_in = len;
    myzlibstream.total_in = len;
    
    myzlibstream.next_out = ( Bytef * ) out;
    myzlibstream.avail_out = curroutlen;
    //myzlibstream.total_out;
    /*
     myzlibstream.msg;
     myzlibstream.state;
     */
    
    myzlibstream.zalloc = ( alloc_func ) NULL;
    myzlibstream.zfree = ( free_func ) NULL;
    myzlibstream.opaque = ( voidpf ) NULL;
    
    /*
     myzlibstream.data_type;
     myzlibstream.adler;
     myzlibstream.reserved;
     */
    
    ret = deflateInit2( &myzlibstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY );
    if ( ret != Z_OK )
    {
        return -1;
    }
    ret = deflate( &myzlibstream, Z_FINISH );
    if ( ret != Z_STREAM_END )
    {
        deflateEnd( &myzlibstream );
        return -1;
    }
    ret = deflateEnd( &myzlibstream );
    if ( ret != Z_OK )
    {
        return -1;
    }
    *outlen = myzlibstream.total_out;
    return 0;
}
