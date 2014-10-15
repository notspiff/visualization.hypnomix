#ifndef __X11KEYCODE_H__
#define __X11KEYCODE_H__

#ifdef __APPLE__
	#define K_ESC 61
	#define K_UP 134
	#define K_DOWN 133 
	#define K_LEFT 132
	#define K_RIGHT 131
	#define K_TAB 56 
	#define K_SPACE 57  
#elif __linux
	#define K_ESC 9
	#define K_UP 111
	#define K_DOWN 116
	#define K_LEFT 114
	#define K_RIGHT 113
	#define K_TAB 23 
	#define K_SPACE 65 
#endif


#endif
