#include <stdio.h>

typedef struct {
   int write:1;
   char name[16];
} LogicalFile;

LogicalFile file[15];

int main() // int argc, char* argv[])
{
   //char* filespec = "0:MYFILE,S,R";
   
   //char filename[16];
   //char filetype[16];
   //char mode[16];
   //char device[16];

   FILE* fp;
   int status;
   
   //sscanf(filespec, "%[^:]:%[^,],%[^,],%s", device, filename, filetype, mode);
   //printf( "In: %s\n", filespec );
   //printf( "device %s, file %s (type %s), mode %s\n", device, filename, filetype, mode );



   fp = fopen( "myfile.txt", "wb+" );
   printf("%d\n", *fp);
   status = fprintf( fp, "hello, world" );
   printf("fprintf() status: %d\n", status);
   fclose(fp);
 
   return 0; 
} 
