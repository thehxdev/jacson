/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Main Module
 * Jacson's public interface
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */


#ifndef JACSON_JACSON_H
#define JACSON_JACSON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Includes
 */
#include "jtypes.h"


/**
 * Macros and constants
 */

#define JACSON_VERSION_MAJOR 0
#define JACSON_VERSION_MINOR 0
#define JACSON_VERSION_PATCH 1
#define JACSON_VERSION "0.0.1"



/**
 * Types
 */

// Jacson main type
typedef struct Jacson Jacson;



/**
 * Module Public API
 */

// Parse raw json data
Jacson *jcsn_parse_json(char *jdata);

// Free all memory used by Jacson
void jcsn_free(Jacson **j);

// get root of AST
Jcsn_JValue *jcsn_ast_root(Jacson *j);

// get a json value from AST
Jcsn_JValue *jcsn_get_value(Jacson *j, char *query);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __JACSON_H__ */
