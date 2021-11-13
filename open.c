#include <stdio.h>

typedef struct {
   int write:1;
   char name[16];
} LogicalFile;

LogicalFile file[15];

int main(int argc, char* argv[])
{
   char* filespec = "0:MYFILE,S,R";
   char* token;
   char filename[16];
   char filetype[16];
   char mode[16];
   char device[16];
   

   sscanf(filespec, "%[^:]:%[^,],%[^,],%s", device, filename, filetype, mode);

   printf( "In: %s\n", filespec );
   printf( "device %s, file %s (type %s), mode %s\n", device, filename, filetype, mode );



   FILE* fp = fopen( "myfile.txt", "wb+" );
   fprintf( fp, "hello, world" );
   fclose(fp);
 
   return 0; 
} 
