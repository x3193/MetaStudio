#include <shlwapi.h>

// link the library
#pragma comment(lib, "shlwapi.lib")


void StripFileName( char *path )
{
	PathRemoveFileSpecA( path );

	if ( strlen( path ) > 0 )
	{
		strcat( path, "\\" );
	}
}

int CheckExtension( char *path, char *ext )
{
	char *check;

	check = PathFindExtension( path );

	if ( check )
	{
		if ( !stricmp( ext, check ) )
		{
			return 1;
		}
	}

	return 0;
}