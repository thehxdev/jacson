/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Parser Module
 * Parse tokens into an AST.
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef __JACSON_PARSER_H
#define __JACSON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Types
 */

typedef struct Jcsn_AST {
    // Root of AST.
    // It can be either a Json Object or Json Array
    Jcsn_JValue *root;
    unsigned long depth;
} Jcsn_AST;



/**
 * Module Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parser_parse_raw(char *jdata);

// Free all memory used by ast
void jcsn_ast_free(Jcsn_AST **ast);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __JACSON_PARSER_H
