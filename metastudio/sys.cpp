
#include <windows.h>

extern "C"
{
	double get_time( void )
	{
		return (double)GetTickCount() / 1000.0;
	}
}