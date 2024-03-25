/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Single header json parser library.
 * Git: https://github.com/thehxdev/jacson
 *
 *
 *
 * MIT License
 * 
 * Copyright (c) 2024 Hossein Khosravi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Includes
 */

// Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Jacson
#include "jacson.h"



/**
 * Macros
 */

// un-comment line below to enable logging (re-compilation needed)
#define __JCSN_LOG__

#ifndef true
    #define true 1
#endif // true

#ifndef false
    #define false 0
#endif // false

// sync left-side pointer with the right-side one
#define sync_ptrs(ptr1, ptr2) (ptr1) = (ptr2)

#ifdef __JCSN_LOG__
    #define JCSN_LOG_ERR(format, ...) (void) fprintf(stderr, "[ERROR] " format, __VA_ARGS__)
    #define JCSN_LOG_INF(format, ...) (void) fprintf(stderr, "[INFO] " format, __VA_ARGS__)
#else
    #define JCSN_LOG_ERR(format, ...)
    #define JCSN_LOG_INF(format, ...)
#endif // __JCSN_LOG__


// Dynamic array default capacity
#define DARR_DEFAULT_CAP 5

// Since `free` ignores NULL pointers, set pointers to `NULL` after
// calling `free` on them to prevent double free error.
#define xfree(ptr) do { free((ptr)); (ptr) = NULL; } while (0)


/**
 * Types
 */

// FIXME: Put all private types from `jacson.h` here...


/**
 * Private API
 */

// check is a character is a whitespace
static inline __attribute__((always_inline)) byte
jcsn_is_whitespace(char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}


static inline __attribute__((always_inline)) void
jcsn_skip_whitespaces(char **ptr) {
    if (*ptr == NULL) return;
    while (**ptr && (jcsn_is_whitespace(**ptr))) *ptr += 1;
}


static inline __attribute__((always_inline)) byte
jcsn_char_isdigit(char ch) {
    return ((ch >= '0') && (ch <= '9'));
}


// `source` is exactly starts with `query`
static char *jcsn_exact_start(char *source, char *query) {
    // example: s = "Hello World!", q = "Hello" -> True
    register char *s = source;
    register char *q = query;

    if (*s != *q)
        return NULL;

    while (*s && *q) {
        if (*s == *q) {
            s += 1;
            q  += 1;
        }
    }

    return (*q == '\0') ? s : NULL;
}


// Parse a string from json data
static char *jcsn_parse_string(Jcsn_Parser *parser) {
    // "a json string example"
    //  ^                    ^
    //  *base                *curr
    //  |--------------------|
    //          delta

    char *str = NULL;
    size_t delta = 0;
    if (*parser->base != '\"')
        goto ret;

    sync_ptrs(parser->curr, parser->base += 1);

    while (*parser->curr != '\"') {
        parser->curr += 1;
        // skip '\"' scape sequance
        if (*parser->curr == '\\' && *(parser->curr+1) == '*')
            parser->curr += 2;
    }

    delta = parser->curr - parser->base;
    str = calloc(delta + 1, sizeof(char));
    if (str == NULL) {
        JCSN_LOG_ERR("%s: Failed to allocate memory for new sub-string\n", __FUNCTION__);
        goto ret;
    }
    strncpy(str, parser->base, delta);

    sync_ptrs(parser->base, parser->curr);
    // put base pointer after last `"` character
    parser->base += 1;
ret:
    return str;
}


// Construct a general json value
static Jcsn_JValue *jcsn_jval_new(enum Jcsn_JVal_T type) {
    Jcsn_JValue *val = malloc(sizeof(*val));
    if (val == NULL)
        goto ret;

    val->type = type;
    val->parsed = false;

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

    (*obj)->len = 0;
    (*obj)->cap = cap;
    // TODO: Handle null pointers
    (*obj)->names = malloc(sizeof(char*) * cap);
    (*obj)->values = malloc(sizeof(Jcsn_JValue*) * cap);

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
    jobj->situation = 1;
    return 0;
}


static byte jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value) {
    Jcsn_JValue **tmp = &jobj->values[jobj->len-1];
    *tmp = value;
    jobj->situation = 0;
    return 0;
}


static Jcsn_JValue *jcsn_jarr_new(size_t cap) {
    Jcsn_JValue *jval = jcsn_jval_new(J_ARRAY);
    if (jval == NULL)
        goto ret;

    Jcsn_JArray **arr = &jval->data.array;

    *arr = malloc(sizeof(**arr));
    // TODO: Handle null pointer
    (*arr)->vals = malloc(sizeof(Jcsn_JValue*) * cap);
    (*arr)->len = 0;
    (*arr)->cap = cap;

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


static byte jcsn_handle_jvalue(Jcsn_Parser *parser, Jcsn_JValue *value) {
    byte stat = 0;
    Jcsn_JValue **scope = &parser->curr_scope;

    if (*scope == NULL) {
        if (value->type == J_OBJECT || value->type == J_ARRAY)
            *scope = value;
        else
            stat = 1;
        goto ret;
    }

    if ((*scope)->type == J_OBJECT)
        if ((*scope)->data.object->situation)
            (void)jcsn_jobj_set_value((*scope)->data.object, value);
        else
            stat = 1;
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
 * Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parse_json(char *jdata) {
    Jcsn_AST *ast = malloc(sizeof(*ast));
    ast->robj = NULL;
    ast->depth = 0;

    if (jdata == NULL)
        return NULL;

    Jcsn_Parser parser = {
        .first = jdata,
        .base  = jdata,
        .curr  = jdata,
        .curr_scope = NULL,
    };

    jcsn_skip_whitespaces(&parser.base);

    char ch, *tmp;
    Jcsn_JValue *new = NULL;
    while ((ch = *parser.base) != '\0') {

        // Handle new json object
        if (ch == '{') {
            JCSN_LOG_INF("Parsing new json object\n", NULL);

            new = jcsn_jobj_new(DARR_DEFAULT_CAP);
            new->parent = parser.curr_scope;

            jcsn_handle_jvalue(&parser, new);
            parser.curr_scope = new;

            parser.base += 1;
        }

        // Handle new json array
        else if (ch == '[') {
            JCSN_LOG_INF("Parsing new json array\n", NULL);

            new = jcsn_jarr_new(DARR_DEFAULT_CAP);
            new->parent = parser.curr_scope;

            jcsn_handle_jvalue(&parser, new);
            parser.curr_scope = new;

            parser.base += 1;
        }

        // Handle json object/array ending
        else if (ch == '}' || ch == ']') {
            JCSN_LOG_INF("Exiting a json scope\n", NULL);

            parser.curr_scope->parsed = true;
            if (parser.curr_scope->parent == NULL)
                ast->robj = parser.curr_scope;
            parser.curr_scope = parser.curr_scope->parent;

            parser.base += 1;
        }

        // Handle strings
        else if (ch == '\"') {
            JCSN_LOG_INF("Parsing new json string\n", NULL);

            tmp = jcsn_parse_string(&parser);
            if (parser.curr_scope->type == J_ARRAY) {
                new = jcsn_jstr_new(tmp);
                new->parent = parser.curr_scope;
                jcsn_handle_jvalue(&parser, new);
            }
            else if (parser.curr_scope->type == J_OBJECT) {
                if (parser.curr_scope->data.object->situation) {
                    new = jcsn_jstr_new(tmp);
                    new->parent = parser.curr_scope;
                    (void)jcsn_handle_jvalue(&parser, new);
                } else {
                    (void)jcsn_jobj_add_name(parser.curr_scope->data.object, tmp);
                }
            }

            jcsn_skip_whitespaces(&parser.base);
        }

        // Handle true/false values (json boolean)
        else if (ch == 't' || ch == 'f') {
            JCSN_LOG_INF("Parsing new json boolean\n", NULL);
            if ((tmp = jcsn_exact_start(parser.base, "true"))) {
                new = jcsn_jval_new(J_BOOL);
                new->parsed = true;
                new->data.boolean = 1;
            } else if ((tmp = jcsn_exact_start(parser.base, "false"))) {
                new = jcsn_jval_new(J_BOOL);
                new->parsed = true;
                new->data.boolean = 0;
            } else {
                // TODO: Handle error
            }

            parser.base = tmp;
            jcsn_handle_jvalue(&parser, new);
            jcsn_skip_whitespaces(&parser.base);
        }

        // Handle json NULL
        else if (ch == 'n') {
            JCSN_LOG_INF("Parsing new json null\n", NULL);
            if ((tmp = jcsn_exact_start(parser.base, "null"))) {
                jcsn_handle_jvalue(&parser, NULL);
                parser.base = tmp;
                jcsn_skip_whitespaces(&parser.base);
            }
            // TODO: Handle error (else block)
        }

        // Handle numbers
        else if (jcsn_char_isdigit(ch)) {
            JCSN_LOG_INF("Parsing new json number\n", NULL);
            // TODO: Parse json number
        }

        // Everything else is an error
        else {
            JCSN_LOG_ERR("%s: Failed to parse json data\n", __FUNCTION__);
            JCSN_LOG_ERR("%s: Invalid character: %c\n", __FUNCTION__, ch);
            goto ret;
        }

        // reached root node in ast (end of parsing)
        if (parser.curr_scope == NULL)
            goto ret;

        if (*parser.base == ',') {
            parser.base += 1;
        }
        else if (*parser.base == ':') {
            parser.base += 1;
        }
        jcsn_skip_whitespaces(&parser.base);
    } // end of while loop

ret:
    return ast;
}


#ifdef __cplusplus
}
#endif // __cplusplus
