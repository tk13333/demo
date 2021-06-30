d
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cdgfile.h"
#include <unistd.h>

/*
int m_bKeyInit=0;
enum { ECB=0, CBC=1, CFB=2 };
#define MAX_KEY_LEN        16 
#define ENCRYPT_GROUP_LEN  16		//must be 2^n
*/

/*************************************************************
函数名称：ReadFileHead
函数说明：读文件头
返回FALSE,失败，TRUE, 成功
***************************************************************/

void DecodeAES(IN char *pBuffer, IN ULONG nLength, IN UCHAR *Key)
{
	ULONG groupsLen,i;
	char key_chain[MAX_KEY_LEN]={0};
	if(0 == nLength) return;
	groupsLen = ENCRYPT_GROUP_LEN*(nLength/ENCRYPT_GROUP_LEN);
	MakeKey((const char *)Key,key_chain, MAX_KEY_LEN, ENCRYPT_GROUP_LEN);
	Decrypt(pBuffer, pBuffer, groupsLen, ECB);

//process the last bytes
	for(i=0;i< (nLength%ENCRYPT_GROUP_LEN); i++)
	{
		*(pBuffer+groupsLen+i) ^= i;
	}
}



int ReadFileHeader(const char* pCDGFilename)
{
    int     ret      = 0; 

    int     rlen     = 0;
    int     wlen     = 0;

    FILE    *infile  = NULL;
    FILE    *outfile = NULL;

    /**************************************************************************/
    // 打开文件
    /**************************************************************************/    
  CDG_FILE_HEADER pHeader = {0};
  if ( ( infile = fopen( pCDGFilename, "rb" ) ) == 0 )
    {
        printf( "Error: EstCdgFileDecrypt : open infile fail\n" );
        ret = TCA_FILE_NOT_OPEN;
        goto out;
     }


 
    if ( ( outfile = fopen( "tmp.txt", "w" ) ) == 0)
    {
        printf( "Error: EstCdgFileDecrypt : open outfile fail\n" );
        ret = TCA_FILE_NOT_OPEN;
        goto out;
    }


   //du qu wnemjiantou 


    printf("infilename  = %s infile = %d , outfile= %d \n",pCDGFilename,infile, outfile);
    rlen = fread((char*)&pHeader, sizeof(char), sizeof(CDG_FILE_HEADER), infile );


    printf(  "1111  rlen = %d, sizeof(CDG_FILE_HEADER) = %d \n", rlen,sizeof(CDG_FILE_HEADER));
  
    if(rlen < sizeof(CDG_FILE_HEADER)) {

           ret =  TCA_INVALID_HEADER;
    }

    //rlen = fread( buffer, sizeof(char), pHeader->dwenleng,   infile );
    // printf("rlen = %d, pHeader->dwenleng = %d \n", rlen,pHeader->dwenleng);
    //  DecodeAES(buffer, sizeof(CDG_FILE_HEADER),(unsigned char *)CDG_FILEHEAD_PASS);

 
   DecodeAES((char*)(&pHeader)+16,sizeof(CDG_FILE_HEADER)-24,(unsigned char *)CDG_FILEHEAD_PASS);

	UCHAR byTransBuf[1024];
	UINT unReadCount = 0;

/*
	unReadCount = fread( byTransBuf, sizeof(char), m_filehead.dwenleng,   infile );
	printf("rlen = %d,m_filehead.dwenleng = %d \n", unReadCount,m_filehead.dwenleng);
	DecodeAES((char*)byTransBuf, m_filehead.dwenleng,(unsigned char *)m_filehead.szpassword);

	wlen = fwrite( byTransBuf, sizeof(char), m_filehead.dwenleng, outfile );
*/


	unReadCount = fread( byTransBuf, sizeof(char), pHeader.dwenleng,   infile );
	printf("rlen = %d,m_filehead.dwenleng = %d \n", unReadCount,pHeader.dwenleng);
	DecodeAES((char*)byTransBuf, pHeader.dwenleng,(unsigned char *)pHeader.szpassword);

	wlen = fwrite( byTransBuf, sizeof(char), pHeader.dwenleng, outfile );


    for( ; ; )
    {

 
        unReadCount = fread( byTransBuf, sizeof(char), 1024, infile );
        printf( "body   len : %d\n", unReadCount );
      
        if ( unReadCount == 0 )
            break;

         wlen = fwrite( byTransBuf, sizeof(char), unReadCount, outfile );
         fsync(outfile);
    }


    rename("tmp.txt", "cdgfile.txt");
    rename("tmp.txt",pCDGFilename);
    


out :



    if ( infile )
        fclose( infile );

    if ( outfile )
        fclose( outfile );


   return ret;  
}

ULONG  IsCDGFile( const char* filename)
{
        int rlen = 0 ;
        int wlen = 0 ;
	ULONG RetVal = TCA_UNKNOW_ERROR;
	CDG_FILE_HEADER header = {0};

	FILE    *infile  = NULL;
	FILE    *outfile = NULL;

	char    *buffer  = NULL;
	buffer = ( char * )malloc( 4096 );

	if ( ( infile = fopen(filename, "rb" ) ) == 0 )
	{
		printf( "Error: EstCdgFileDecrypt : open infile fail\n" );
		RetVal = TCA_FILE_NOT_OPEN ;
		goto out;
	}


	RetVal = ReadFileHeader(filename);
	if( RetVal != TCA_SUCCESS )  {
	    RetVal = TCA_INVALID_HEADER  ;
		goto out;
	}
out :
    if ( buffer )
        free( buffer );

    if ( infile )
        fclose( infile );

    if ( outfile )
        fclose( outfile );

	return RetVal;    

}

/*******************************************************************************/
//
// End of File 
//
/*******************************************************************************/
