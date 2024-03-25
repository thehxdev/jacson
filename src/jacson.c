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
    // example: "Hello World!", "Hello" -> True
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


static Jcsn_JValue *jcsn_jobj_new(size_t cap) {
    // TODO: Impement json object constructor
    (void)cap;
    return NULL;
}


static byte jcsn_jobj_add_name(Jcsn_JObject *jobj, const char *name) {
    // TODO: Implement jcsn_jobj_add_name
    (void)jobj; (void)name;
    return 0;
}


static byte jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value) {
    // TODO: Impement json object value setter
    (void)jobj; (void)value;
    return 0;
}


static Jcsn_JValue *jcsn_jarr_new(size_t cap) {
    Jcsn_JValue *jval = malloc(sizeof(*jval));
    if (jval == NULL)
        goto ret;

    jval->type = J_ARRAY;
    jval->parsed = false;
    Jcsn_JArray **arr = &jval->data.array;

    *arr = malloc(sizeof(Jcsn_JArray));
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
        // TODO: Handle null pointer
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

    if ((*scope)->type == J_OBJECT) {
        if ((*scope)->data.object->situation) {
            // TODO: Implement jcsn_jobj_set_value
            (void)jcsn_jobj_set_value((*scope)->data.object, value);
            (*scope)->data.object->situation = 0;
        } else
            stat = 1;
    }
    else if ((*scope)->type == J_ARRAY) {
        // TODO: Implement jcsn_jarr_append
        (void)jcsn_jarr_append((*scope)->data.array, value);
    }

ret:
    return stat;
}


static Jcsn_JValue *jcsn_jstr_new(const char *str) {
    Jcsn_JValue *val = malloc(sizeof(*val));
    if (val == NULL)
        goto ret;

    val->type = J_STRING;
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
            jcsn_skip_whitespaces(&parser.base);
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
            new = jcsn_jstr_new(tmp);
            new->parent = parser.curr_scope;
            jcsn_handle_jvalue(&parser, new);

            jcsn_skip_whitespaces(&parser.base);
        }

        // Handle true/false values (json boolean)
        else if (ch == 't' || ch == 'f') {
            // TODO: Parse json boolean
        }

        // Handle json NULL
        else if (ch == 'n') {
            // TODO: Parse json null
        }

        // Handle numbers
        else if (jcsn_char_isdigit(ch)) {
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
            // Handle json object
        }
        jcsn_skip_whitespaces(&parser.base);
    } // end of while loop

ret:
    return ast;
}


#ifdef __cplusplus
}
#endif // __cplusplus
