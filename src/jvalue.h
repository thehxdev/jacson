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

#ifndef __JACSON_JVALUE_H
#define __JACSON_JVALUE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <jacson/jtypes.h>


/**
 * Module Public API
 */

// Construct a new general json value
Jcsn_JValue *jcsn_jval_new(enum Jcsn_JVal_T type);

// Construct a new json object
Jcsn_JValue *jcsn_jobj_new(size_t cap);

// Add a name to json object
byte jcsn_jobj_add_name(Jcsn_JObject *jobj, const char *name);

// Set a neme's value in json object
byte jcsn_jobj_set_value(Jcsn_JObject *jobj, Jcsn_JValue *value);

// Construct a new json array
Jcsn_JValue *jcsn_jarr_new(unsigned long cap);

// Append a json value to json array
byte jcsn_jarr_append(Jcsn_JArray *jarr, Jcsn_JValue *value);

// Construct a new json string
Jcsn_JValue *jcsn_jstr_new(const char *str);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __JACSON_JVALUE_H
