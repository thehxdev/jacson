/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Json Value Module
 * Json value related functionalities
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
#include <stdbool.h>
#include <string.h>

// Jacson
#include "log.h"
#include "mem.h"
#include "jvalue.h"



/**
 * Module Public API
 */

Jcsn_JValue *jcsn_jval_new(enum Jcsn_JVal_T type) {
    Jcsn_JValue *val = malloc(sizeof(*val));
    if (!val)
        goto ret;

    *val = (Jcsn_JValue){
        .type = type,
        //.parsed = false,
        .parent = NULL,
    };

ret:
    return val;
}


// Construct a new json object
Jcsn_JValue *jcsn_jobj_new(void) {
    Jcsn_JValue *jval = jcsn_jval_new(J_OBJECT);
    if (!jval)
        return jval;

    Jcsn_JObject *obj = &jval->data.object;

    *obj = (Jcsn_JObject){
        .len = 0,
        .cap = 4,
    };

    obj->names = malloc(sizeof(*obj->names) * obj->cap);
    obj->values = malloc(sizeof(*obj->values) * obj->cap);
    if (!obj->names || !obj->values) {
        xfree(obj->names);
        xfree(obj->values);
        xfree(jval);
    }

    return jval;
}


int jcsn_jobj_add_name(Jcsn_JObject *jobj, const char *name) {
    if (jobj->len == jobj->cap) {
        jobj->cap <<= 1;
        void *tmp = realloc(jobj->names, sizeof(*jobj->names) * jobj->cap);
        if (!tmp)
            return 0;
        jobj->names = tmp;
        tmp = realloc(jobj->values, sizeof(*jobj->values) * jobj->cap);
        if (!tmp)
            return 0;
        jobj->values = tmp;
    }
    jobj->names[jobj->len] = (char*)name;
    jobj->len += 1;
    return 1;
}


Jcsn_JValue *jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value) {
    Jcsn_JValue *last = &jobj->values[jobj->len - 1];
    memmove(last, value, sizeof(*value));
    return last;
}


Jcsn_JValue *jcsn_jarr_new(void) {
    Jcsn_JValue *jval = jcsn_jval_new(J_ARRAY);
    if (!jval)
        goto ret;
    Jcsn_JArray *arr = &jval->data.array;

    *arr = (Jcsn_JArray){
        .len = 0,
        .cap = 4,
    };
    arr->vals = malloc(sizeof(*arr->vals) * arr->cap);

ret:
    return jval;
}


Jcsn_JValue *jcsn_jarr_append(Jcsn_JArray *jarr, Jcsn_JValue *value) {
    if (jarr->len == jarr->cap) {
        jarr->cap <<= 1;
        void *tmp = realloc(jarr->vals, jarr->cap * sizeof(*jarr->vals));
        if (!tmp) {
            JCSN_LOG_ERR("%s: Failed to grow json array's memory\n", __FUNCTION__);
            return NULL;
        }
        jarr->vals = tmp;
    }
    Jcsn_JValue *last = &jarr->vals[jarr->len];
    memmove(last, value, sizeof(*value));
    jarr->len += 1;
    return last;
}


// Construct a new json string
Jcsn_JValue *jcsn_jstr_new(const char *str) {
    Jcsn_JValue *val = jcsn_jval_new(J_STRING);
    val->data.string = (char*)str;
    return val;
}



#ifdef __cplusplus
}
#endif // __cplusplus
