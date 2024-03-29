/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Main Module
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
#include <jacson/jacson.h>



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

// Construct a general json value
static Jcsn_JValue *jcsn_jval_new(enum Jcsn_JVal_T type) {
    Jcsn_JValue *val = malloc(sizeof(*val));
    if (val == NULL)
        goto ret;

    *val = (Jcsn_JValue) {
        .type = type,
        .parsed = false,
        .parent = NULL,
    };

ret:
    return val;
}


// Construct a new json object
static Jcsn_JValue *jcsn_jobj_new(size_t cap) {
    Jcsn_JValue *jval = jcsn_jval_new(J_OBJECT);
    if (jval == NULL)
        goto ret;

    Jcsn_JObject **obj = &jval->data.object;

    *obj = malloc(sizeof(**obj));
    if (*obj == NULL) {
        xfree(jval);
        goto ret;
    }

    **obj = (Jcsn_JObject) {
        .len = 0,
        .cap = cap,
        // TODO: Handle null pointer
        .names = malloc(sizeof(char*) * cap),
        .values = malloc(sizeof(Jcsn_JValue*) * cap),
    };

ret:
    return jval;
}


static byte jcsn_jobj_add_name(Jcsn_JObject *jobj, const char *name) {
    if ((jobj->len % jobj->cap) == 0) {
        size_t sum = (jobj->len + jobj->cap);

        // TODO: Handle null pointers
        jobj->names = realloc(jobj->names, sum * sizeof(char*));
        jobj->values = realloc(jobj->values, sum * sizeof(Jcsn_JValue*));
        if (jobj->names == NULL || jobj->values == NULL)
            return 1;
    }

    char **tmp = &jobj->names[jobj->len];
    *tmp = (char*)name;
    jobj->len += 1;
    return 0;
}


static byte jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value) {
    Jcsn_JValue **tmp = &jobj->values[jobj->len-1];
    *tmp = value;
    return 0;
}


static Jcsn_JValue *jcsn_jarr_new(size_t cap) {
    Jcsn_JValue *jval = jcsn_jval_new(J_ARRAY);
    if (jval == NULL)
        goto ret;

    Jcsn_JArray **arr = &jval->data.array;

    *arr = malloc(sizeof(**arr));
    **arr = (Jcsn_JArray) {
        .len = 0,
        .cap = cap,
        // TODO: Handle null pointer
        .vals = malloc(sizeof(Jcsn_JValue*) * cap),
    };

ret:
    return jval;
}


static byte jcsn_jarr_append(Jcsn_JArray *jarr, Jcsn_JValue *value) {
    if ((jarr->len % jarr->cap) == 0) {
        size_t new_size = (jarr->len + jarr->cap) * sizeof(Jcsn_JValue*);
        jarr->vals = realloc(jarr->vals, new_size);
        if (jarr->vals == NULL) {
            JCSN_LOG_ERR("%s: Failed to grow json array's memory\n", __FUNCTION__);
            return 1;
        }
    }

    Jcsn_JValue **tmp = &jarr->vals[jarr->len];
    *tmp = value;
    jarr->len += 1;
    return 0;
}


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


// Construct a new json string
static Jcsn_JValue *jcsn_jstr_new(const char *str) {
    Jcsn_JValue *val = jcsn_jval_new(J_STRING);
    if (val == NULL)
        goto ret;

    val->data.string = (char*)str;
    val->parsed = true;

ret:
    return val;
}


/**
 * Module Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parse_json(char *jdata) {
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
                if (parser.scope->parent == NULL)
                    ast->root = parser.scope;
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

    jcsn_tlist_free(&tlist, false);
    return ast;
}


void jcsn_ast_print(Jcsn_AST *ast) {
    // TODO: Implement ast printer
    (void)ast;
}


void jcsn_ast_free(Jcsn_AST **ast) {
    // TODO: Implement jcsn_ast_free
    (void)ast;
}



#ifdef __cplusplus
}
#endif // __cplusplus

