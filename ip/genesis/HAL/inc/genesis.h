/*
 * Driver for the Genesis Controller Adapter
 */

#ifndef _GENESIS_H_
#define _GENESIS_H_

#include <stddef.h>
#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct genesis_dev_t
{
	alt_dev dev;
	unsigned int base;
} genesis_dev_t;

typedef struct genesis_controller_t
{
	char a : 1;
	char b : 1;
	char c : 1;
	char start : 1;
	char up : 1;
	char down : 1;
	char left : 1;
	char right : 1;
} genesis_controller_t;

genesis_dev_t *genesis_open_dev(const char *name);
genesis_controller_t genesis_get(unsigned char player);

/* Macros used by alt_sys_init */
#define GENESIS_INSTANCE(name, device)	\
  static genesis_dev_t device =		\
  {                                                 	\
    {                                               	\
      ALT_LLIST_ENTRY,                              	\
      name##_NAME,                                  	\
      NULL , /* open */		\
      NULL , /* close */		\
      NULL, /* read */		\
      NULL, /* write */		\
      NULL , /* lseek */		\
      NULL , /* fstat */		\
      NULL , /* ioctl */		\
    },                                              	\
	name##_BASE,                                	\
  }

#define GENESIS_INIT(name, device) \
{	\
    alt_dev_reg(&device.dev);                          	\
}
	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GENESIS_H_ */
