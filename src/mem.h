/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Memory Module
 * Memory related functionalities
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef __JACSON_MEM_H
#define __JACSON_MEM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Macros
 */

// Since `free` ignores NULL pointers, set pointers to `NULL` after
// calling `free` on them to prevent double free error.
#define xfree(ptr)		\
	do {				\
		free((ptr));	\
		(ptr) = NULL;	\
	} while (0)



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_MEM_H
