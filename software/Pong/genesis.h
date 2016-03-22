/*
 * Helper methods to return handy struct for reading genesis
 * controller states.
 */

#include <io.h>
#include <system.h>
#include <sys/alt_stdio.h>

typedef struct Controllers{
	int a, b, c, up, down, left, right, start;
} Controller;

Controller get_player1();

Controller get_player2();
