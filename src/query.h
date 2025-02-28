/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Query Module
 * Query data from AST
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef JACSON_QUERY_H
#define JACSON_QUERY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Includes
 */

// Jacson
#include <jacson/jtypes.h>



/**
 * Module Public API
 */

// Get a value from AST
Jcsn_JValue *jcsn_query_value(Jcsn_JValue *root, const char *query);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_QUERY_H
