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
// #define __JCSN_LOG__

#ifndef true
    #define true 1
#endif // true

#ifndef false
    #define false 0
#endif // false

// sync left-side pointer with the right-side one
#define sync_ptrs(ptr1, ptr2) (ptr1) = (ptr2)

#ifdef __JCSN_LOG__
    #define JCSN_LOG_ERR(format, ...) (void) fprintf(stderr, "[ERROR]" format, __VA_ARGS__)
#else
    #define JCSN_LOG_ERR(format, ...)
#endif // __JCSN_LOG__


/**
 * Types
 */

// a signed 8-bit integer
typedef char byte;

// use normal character pointer as a string
typedef char* Jcsn_JString;


// json value type
enum Jcsn_JVal_T {
    J_OBJECT,
    J_ARRAY,
    J_STRING,
    J_NUMBER,
    J_BOOL,
    J_NULL,
};

union __Jcsn_JValue {
    struct Jcsn_JArray *array;
    struct Jcsn_JObject *object;
    Jcsn_JString string;
    int  boolean;
    long integer; // Handle all numbers as integers for now...

    // Don't handle floating point numbers yet...
    double real; // Fortran programmer vibe, huh? :)
};

typedef struct Jcsn_JValue {
    union __Jcsn_JValue *data;
    enum Jcsn_JVal_T type;

    // Any json value is part of a json object or a json array.
    // `parent` is a pointer to the json object or json array
    // that this value belongs to.
    struct Jcsn_JValue *parent;
    byte parsed;
} Jcsn_JValue;


typedef struct Jcsn_JArray {
    Jcsn_JValue **vals;
    unsigned long len;
    unsigned long cap;
} Jcsn_JArray;


// Json object is just a map between strings and Json values.
// So I defined a json object as a dynamic array that each member
// of `names` field is mapped to exact same index in `values` field.
// In this form, we created a map from strings to value they refer to.
typedef struct Jcsn_JObject {
    Jcsn_JValue **values;
    char **names;
    unsigned long len;
    unsigned long cap;
} Jcsn_JObject;


struct Jcsn_AST {
    // Root of AST.
    // It can be either a Json Object or Json Array
    Jcsn_JValue *robj;
    unsigned long depth;
};


typedef struct Jcsn_Parser {
    // Since we're working with pointer arithmic, keep a pointer
    // first character in json data to prevent out of bound access
    // to memory locations if parser wants to go backward.
    char *first;

    // Pointer to base character
    // (Used to extract tokens (sub-strings) with `curr` field)
    char *base;

    // Pointer to current character in parser
    char *curr;

    // Current data collection that we append data to it.
    // A data collection in json is either a json object
    // or a json array.
    // Just keep a pointer to it, to know where we add
    // the parsed data.
    // It's like scope in programming languages.
    Jcsn_JValue *curr_scope;
} Jcsn_Parser;



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
    // "a json string"
    //  ^            ^
    //  *base        *curr

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
    parser->base += 1;
ret:
    return str;
}



/**
 * Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parse_json(char *jdata) {
    if (jdata == NULL)
        return NULL;

    Jcsn_Parser parser = {
        .first = jdata,
        .base  = jdata,
        .curr  = jdata,
        .curr_scope = NULL,
    };

    return NULL;
}


#ifdef __cplusplus
}
#endif // __cplusplus
