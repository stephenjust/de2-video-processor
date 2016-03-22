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

Controller get_player1(){
		Controller player1;
		int controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
		if (controller_value & (1 << 0))
			player1.up = 1;
		else
			player1.up = 0;
		if (controller_value & (1 << 1))
			player1.down = 1;
		else
			player1.down = 0;
		if (controller_value & (1 << 2))
			player1.left = 1;
		else
			player1.left = 0;
		if (controller_value & (1 << 3))
			player1.right = 1;
		else
			player1.right = 0;
		if (controller_value & (1 << 4))
			player1.a = 1;
		else
			player1.a = 0;
		if (controller_value & (1 << 5))
			player1.b = 1;
		else
			player1.b = 0;
		if (controller_value & (1 << 6))
			player1.c = 1;
		else
			player1.c = 0;
		if (controller_value & (1 << 7))
			player1.start = 1;
		else
			player1.start = 0;

		return player1;
	}

Controller get_player2(){
		Controller player2;
		int controller_value = IORD_32DIRECT(GENESIS_0_BASE, 0);
		if (controller_value & (1 << 10))
			player2.up = 1;
		else
			player2.up = 0;
		if (controller_value & (1 << 11))
			player2.down = 1;
		else
			player2.down = 0;
		if (controller_value & (1 << 12))
			player2.left = 1;
		else
			player2.left = 0;
		if (controller_value & (1 << 13))
			player2.right = 1;
		else
			player2.right = 0;
		if (controller_value & (1 << 14))
			player2.a = 1;
		else
			player2.a = 0;
		if (controller_value & (1 << 15))
			player2.b = 1;
		else
			player2.b = 0;
		if (controller_value & (1 << 16))
			player2.c = 1;
		else
			player2.c = 0;
		if (controller_value & (1 << 17))
			player2.start = 1;
		else
			player2.start = 0;

		return player2;
	}
