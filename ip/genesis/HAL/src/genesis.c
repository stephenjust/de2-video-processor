/*
 * Driver for the Genesis Controller Adapter
 */

#include <io.h>
#include <priv/alt_file.h>
#include "genesis.h"

static genesis_dev_t *device_pointer = NULL;

#define PLAYER1_UP_SHIFT 0
#define PLAYER1_DOWN_SHIFT 1
#define PLAYER1_LEFT_SHIFT 2
#define PLAYER1_RIGHT_SHIFT 3
#define PLAYER1_A_SHIFT 4
#define PLAYER1_B_SHIFT 5
#define PLAYER1_C_SHIFT 6
#define PLAYER1_START_SHIFT 7

#define PLAYER2_UP_SHIFT 10
#define PLAYER2_DOWN_SHIFT 11
#define PLAYER2_LEFT_SHIFT 12
#define PLAYER2_RIGHT_SHIFT 13
#define PLAYER2_A_SHIFT 14
#define PLAYER2_B_SHIFT 15
#define PLAYER2_C_SHIFT 16
#define PLAYER2_START_SHIFT 17

/**
 * Open a Genesis Controller decoder device
 *
 * This device may be connected to up to two controllers.
 * Only one device may be open at a time.
 */
genesis_dev_t *genesis_open_dev(const char *name)
{
	genesis_dev_t *dev = (genesis_dev_t *) alt_find_dev(name, &alt_dev_list);

	if (dev != NULL)
	{
		device_pointer = dev;
	}
	return dev;
}

/**
 * Get the value of a genesis controller
 *
 * Arguments:
 *     player: GENESIS_PLAYER_1 or GENESIS_PLAYER_2
 */
genesis_controller_t genesis_get(unsigned char player)
{
	genesis_controller_t state = {};
	unsigned int controller_value = IORD_32DIRECT(device_pointer->base, 0);
	if (player == GENESIS_PLAYER_1)
	{
		state.up    = !!(controller_value & (1 << PLAYER1_UP_SHIFT));
		state.down  = !!(controller_value & (1 << PLAYER1_DOWN_SHIFT));
		state.left  = !!(controller_value & (1 << PLAYER1_LEFT_SHIFT));
		state.right = !!(controller_value & (1 << PLAYER1_RIGHT_SHIFT));
		state.a     = !!(controller_value & (1 << PLAYER1_A_SHIFT));
		state.b     = !!(controller_value & (1 << PLAYER1_B_SHIFT));
		state.c     = !!(controller_value & (1 << PLAYER1_C_SHIFT));
		state.start = !!(controller_value & (1 << PLAYER1_START_SHIFT));
	}
	else if (player == GENESIS_PLAYER_2)
	{
		state.up    = !!(controller_value & (1 << PLAYER2_UP_SHIFT));
		state.down  = !!(controller_value & (1 << PLAYER2_DOWN_SHIFT));
		state.left  = !!(controller_value & (1 << PLAYER2_LEFT_SHIFT));
		state.right = !!(controller_value & (1 << PLAYER2_RIGHT_SHIFT));
		state.a     = !!(controller_value & (1 << PLAYER2_A_SHIFT));
		state.b     = !!(controller_value & (1 << PLAYER2_B_SHIFT));
		state.c     = !!(controller_value & (1 << PLAYER2_C_SHIFT));
		state.start = !!(controller_value & (1 << PLAYER2_START_SHIFT));
	}
	return state;
}
