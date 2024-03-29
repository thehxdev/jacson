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
#include "log.h"
#include "mem.h"
#include "types.h"
#include "lexer.h"
#include "validator.h"
#include "jvalue.h"
#include "parser.h"



/**
 * Macros and Constants
 */

// Dynamic array default capacity
#define DARR_DEF_CAP 20



/**
 * Types
 */

typedef struct Jcsn_Parser {
    // Pointer to previous token
    Jcsn_Token *prev;

    // Pointer to current token
    Jcsn_Token *curr;

    // Pointer to next token
    Jcsn_Token *next;

    // Current data collection that we append data to it.
    // A data collection in json is either a json object or a json array.
    // Just keep a pointer to it, to know where we add the parsed data.
    // It's like scope in programming languages.
    Jcsn_JValue *scope;
} Jcsn_Parser;



/**
 * Module Private API
 */

// stat = 1 -> OK
// stat = 0 -> ERROR
static byte jcsn_handle_jvalue(Jcsn_Parser *parser, Jcsn_JValue *value) {
    byte stat = 1;
    Jcsn_JValue **scope = &parser->scope;

    if (*scope == NULL) {
        if (value->type == J_OBJECT || value->type == J_ARRAY)
            *scope = value;
        else
            stat = 0;
        goto ret;
    }

    value->parent = *scope;
    if ((*scope)->type == J_OBJECT)
        if (parser->prev->type == ':')
            (void)jcsn_jobj_set_value((*scope)->data.object, value);
        else 
            stat = 0;
    else if ((*scope)->type == J_ARRAY)
        (void)jcsn_jarr_append((*scope)->data.array, value);

ret:
    return stat;
}



/**
 * Module Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parser_parse_raw(char *jdata) {
    Jcsn_TList *tlist = jcsn_tokenize_json(jdata);
    if (!jcsn_validate_tokens(tlist)) {
        JCSN_LOG_ERR("Provided json data is not valid\n", NULL);
        JCSN_LOG_INF("Returning NULL\n", NULL);
        return NULL;
    }

    long len = (long)tlist->len, i;
    Jcsn_JValue *val = NULL;
    register Jcsn_Token **tks = tlist->tokens;

    Jcsn_Parser parser = { 0 };
    Jcsn_AST *ast = malloc(sizeof(*ast));
    *ast = (Jcsn_AST) {
        .root = NULL,
        .depth = 0,
    };


    for (i = 0; i < len; i++) {
        parser.prev = jcsn_tlist_get(tks, len, i-1);
        parser.curr = jcsn_tlist_get(tks, len, i);
        parser.next = jcsn_tlist_get(tks, len, i+1);

        switch (parser.curr->type) {
            case '{': {
                val = jcsn_jobj_new(DARR_DEF_CAP);
                jcsn_handle_jvalue(&parser, val);
                parser.scope = val;
                ast->depth += 1;
            }
            break;

            case '[': {
                val = jcsn_jarr_new(DARR_DEF_CAP);
                jcsn_handle_jvalue(&parser, val);
                parser.scope = val;
                ast->depth += 1;
            }
            break;

            case '}':
            case ']': {
                parser.scope->parsed = true;
                if (parser.scope->parent == NULL) {
                    ast->root = parser.scope;
                    goto ret;
                }
                parser.scope = parser.scope->parent;
            }
            break;

            case TK_STRING: {
                if (parser.scope->type == J_OBJECT) { 
                    if (parser.next->type == ':')
                        jcsn_jobj_add_name(parser.scope->data.object, parser.curr->value.string);
                    else {
                        val = jcsn_jstr_new(parser.curr->value.string);
                        jcsn_handle_jvalue(&parser, val);
                    }
                } else if (parser.scope->type == J_ARRAY) {
                    val = jcsn_jval_new(J_STRING);
                    val->parent = parser.scope;
                    val->data.string = parser.curr->value.string;
                    jcsn_handle_jvalue(&parser, val);
                }
            }
            break;

            case TK_BOOL: {
                val = jcsn_jval_new(J_BOOL);
                val->data.boolean = parser.curr->value.boolean;
                jcsn_handle_jvalue(&parser, val);
            }
            break;

            case TK_NULL: {
                val = jcsn_jval_new(J_NULL);
                jcsn_handle_jvalue(&parser, val);
            }
            break;

            case TK_INTEGER: {
                val = jcsn_jval_new(J_INTEGER);
                val->data.integer = parser.curr->value.integer;
                jcsn_handle_jvalue(&parser, val);
            }
            break;

            case TK_DOUBLE: {
                val = jcsn_jval_new(J_REAL);
                val->data.real = parser.curr->value.real;
                jcsn_handle_jvalue(&parser, val);
            }
            break;

            default: break;
        } // end switch (curr->type)
    } // end for loop

ret:
    jcsn_tlist_free(&tlist, false);
    return ast;
}


void jcsn_ast_free(Jcsn_AST **ast) {
    if (*ast == NULL)
        return;

    long i;
    Jcsn_JObject *obj;
    Jcsn_JArray *arr;
    Jcsn_JValue *scope = (*ast)->root, *curr = NULL, *parent;

again:
    while (1) {
        if (scope->type == J_OBJECT) { // handle json object
            obj = scope->data.object;
            
            while ((i = obj->len -= 1, i >= 0)) {
                xfree(obj->names[i]);
                curr = obj->values[i];
                switch (curr->type) {
                    case J_OBJECT:
                    case J_ARRAY:
                        scope = curr;
                        goto again;
                        break;

                    case J_STRING: {
                        xfree(curr->data.string);
                        xfree(curr);
                    } break;

                    default:
                        xfree(curr);
                        break;
                } // end switch (curr->type)
            } // end while loop
            xfree(obj->values);
            xfree(obj->names);
            xfree(obj);
            parent = scope->parent;
            xfree(scope);
            scope = parent;
        }
        else { // handle json array
            arr = scope->data.array;
            
            while ((i = arr->len -= 1, i >= 0)) {
                curr = arr->vals[i];
                switch (curr->type) {
                    case J_OBJECT:
                    case J_ARRAY:
                        scope = curr;
                        goto again;
                        break;

                    case J_STRING: {
                        xfree(curr->data.string);
                        xfree(curr);
                    } break;

                    default:
                        xfree(curr);
                        break;
                } // end switch (curr->type)
            } // end while loop
            xfree(arr->vals);
            xfree(arr);
            parent = scope->parent;
            xfree(scope);
            scope = parent;
        }

        if (scope == NULL)
            break;
    } // end while (scope) 
    xfree(*ast);
}



#ifdef __cplusplus
}
#endif // __cplusplus
