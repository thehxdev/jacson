/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Json Value Module
 * Define json values and related functionalities.
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
#include "types.h"
#include "log.h"
#include "mem.h"
#include "jvalue.h"



/**
 * Module Public API
 */

Jcsn_JValue *jcsn_jval_new(enum Jcsn_JVal_T type) {
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
Jcsn_JValue *jcsn_jobj_new(size_t cap) {
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


byte jcsn_jobj_add_name(Jcsn_JObject *jobj, const char *name) {
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


byte jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value) {
    Jcsn_JValue **tmp = &jobj->values[jobj->len-1];
    *tmp = value;
    return 0;
}


Jcsn_JValue *jcsn_jarr_new(size_t cap) {
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


byte jcsn_jarr_append(Jcsn_JArray *jarr, Jcsn_JValue *value) {
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


// Construct a new json string
Jcsn_JValue *jcsn_jstr_new(const char *str) {
    Jcsn_JValue *val = jcsn_jval_new(J_STRING);
    if (val == NULL)
        goto ret;

    val->data.string = (char*)str;
    val->parsed = true;

ret:
    return val;
}



