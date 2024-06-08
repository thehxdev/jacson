/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Validator Module
 * Validate tokens to match json syntax rules.
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */


/**
 * What we're trying to validate?
 *
 *  1. First token in token list must be '{' or '['. Any valid json data starts by
 *     a json object or json array. everything else is invalid.
 *
 *  2. After each json value must be a ',' value except for the last one in json object/array.
 *
 *  3. After each string in json object that comes after a ',' character, must be
 *     a ':' character.
 *
 *  4. Check for '{}' and '[]' characters to match each other.
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* FIXME:
 * Validator is very simple at this point and does not check
 * everything in json syntax. This is a note to improve the 
 * validator to do more advanced checks.
 */


/**
 * Includes
 */

// Standard Library
#ifdef __JCSN_TRACE__
    #include <stdio.h>
#endif // __JCSN_TRACE__
#include <stdlib.h>

// Jacson
#include "types.h"
#include "lexer.h"
#include "log.h"


/**
 * Module Private API
 */



/**
 * Module Public API
 */

// validate json tokens
// 0 -> found invalid token(s)
// 1 -> everything is ok
byte jcsn_validate_tokens(Jcsn_TList *tlist) {
    if (tlist == NULL)
        return 0;

    byte stat = 1;
    size_t len = tlist->len, i;
    long brace_nest = 0, bracket_nest = 0;
    register Jcsn_Token *tks = tlist->tokens, *curr = &tks[0], *prev, *next;

    // Check first token
    // It must be one of '[' or '{' characters.
    switch(curr->type) {
        case '{':
        case '[':
            break;

        default: {
            JCSN_LOG_ERR("Json data is not valid\n", NULL);
            JCSN_LOG_ERR("Expected \'{\' or \'[\' characters as first token\n", NULL);
            return 0;
        }
    }


    for (i = 0; i < len; i++) {
        prev = jcsn_tlist_get(tks, len, i-1);
        curr = jcsn_tlist_get(tks, len, i);
        next = jcsn_tlist_get(tks, len, i+1);

        switch (curr->type) {
            case '{': {
                if (next->type != TK_STRING && next->type != '}') {
                    JCSN_LOG_ERR("Expected json string or \'}\' after \'{\' character\n", NULL);
                    stat = 0;
                    goto ret;
                }
                brace_nest += 1;
            } break;

            case '[': {
                bracket_nest += 1;
            } break;

            case '}': {
                if (prev->type == ',') {
                    JCSN_LOG_ERR("Found extra \',\' character befor json object ending\n", NULL);
                    stat = 0;
                    goto ret;
                }
                brace_nest -= 1;
            } break;

            case ']': {
                if (prev->type == ',') {
                    JCSN_LOG_ERR("Found extra \',\' character befor json array ending\n", NULL);
                    stat = 0;
                    goto ret;
                }
                bracket_nest -= 1;
            } break;

            case ':': {
                if (prev->type != TK_STRING) {
                    JCSN_LOG_ERR("Expected json string befor \':\' character\n", NULL);
                    stat = 0;
                    goto ret;
                }
                if (next->type == ',') {
                    JCSN_LOG_ERR("Expected json value after \':\' character but \',\' found\n", NULL);
                    stat = 0;
                    goto ret;
                }
            } break;

            default: break;
        } // end switch(curr->type)
    } // end for loop

    if (brace_nest != 0 || bracket_nest != 0) {
        JCSN_LOG_ERR("Extra braces/brackets found in json data\n", NULL);
        stat = 0;
    }

ret:
    return stat;
}


#ifdef __cplusplus
}
#endif // __cplusplus
