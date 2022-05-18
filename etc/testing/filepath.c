// This program demonstrates how _fullpath
// creates a full path from a partial path.

#include <stdio.h>
#include <stdlib.h>

void PrintFullPath( char * partialPath )
{
   char full[_MAX_PATH];
   if( _fullpath( full, partialPath, _MAX_PATH ) != NULL )
      printf( "Full path is: %s\n", full );
   else
      printf( "Invalid path\n" );
}

int main( void )
{
   PrintFullPath( "..\\..\\assets\\backgrounds\\blue.mp4" );
   system("pause");
}