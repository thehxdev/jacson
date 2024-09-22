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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Includes
 */

// Standard Library
#ifdef __JCSN_TRACE__
    #include <stdio.h>
#endif // __JCSN_TRACE__
#include <stdlib.h>

// Jacson
#include "mem.h"
#include "jvalue.h"
#include "parser.h"
#include "query.h"
#include <jacson/jacson.h>



/**
 * Types
 */

struct Jacson {
    Jcsn_AST *ast;
};


Jacson *jcsn_parse_json(char *jdata) {
    Jacson *j = malloc(sizeof(*j));
    if (!j)
        return NULL;
    
    *j = (Jacson) {
        .ast = jcsn_parser_parse_raw(jdata),
    };

    return j;
}


void jcsn_free(Jacson *j) {
    jcsn_ast_free(j->ast);
    xfree(j);
}


Jcsn_JValue *jcsn_ast_root(Jacson *j) {
    return (j->ast) ? j->ast->root : NULL;
}


Jcsn_JValue *jcsn_get_value(Jacson *j, char *query) {
    return jcsn_query_value(j->ast->root, query);
}


#ifdef __cplusplus
}
#endif // __cplusplus

