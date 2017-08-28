#include <windows.h>

double get_time( void )
{
	return (double)GetTickCount() / 1000.0;
}

void sleep( int t )
{
	Sleep( t );
}