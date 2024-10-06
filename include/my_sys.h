

#ifndef MY_SYS_H
#define MY_SYS_H

typedef unsigned int byte;

#ifdef __linux
#include <unistd.h>
#define MY_TEXT(x)  x
#else
	#define MY_TEXT 
#endif

#endif