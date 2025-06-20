/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Json Value Types Module
 * Type definitions for json values.
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef JACSON_JTYPES_H
#define JACSON_JTYPES_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>


/**
 * Types
 */

// json value type
enum Jcsn_JVal_T {
    J_OBJECT,
    J_ARRAY,
    J_STRING,
    J_INTEGER,
    J_REAL, // double
    J_BOOL,
    J_NULL,
};

struct Jcsn_JArray {
    struct Jcsn_JValue *vals;
    unsigned long len;
    unsigned long cap;
};

struct Jcsn_JObject {
    struct Jcsn_JValue *values;
    char **names;
    unsigned long len;
    unsigned long cap;
};

typedef struct Jcsn_JValue {
    //bool parsed;

    enum Jcsn_JVal_T type;

    // Any json value is part of a json object or a json array.
    // `parent` is a pointer to the json object or json array
    // that this value belongs to.
    struct Jcsn_JValue *parent;

    union {
        struct Jcsn_JArray array;
        struct Jcsn_JObject object;
        char *string;
        double real;
        long integer;
        bool boolean;
    } data;
} Jcsn_JValue;


typedef struct Jcsn_JArray Jcsn_JArray;


// Json object is just a map between strings and Json values.
// So I defined a json object as a dynamic array that each member
// of `names` field is mapped to exact same index in `values` field.
// In this form, we created a map from strings to value they refer to.
typedef struct Jcsn_JObject Jcsn_JObject;



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_JTYPES_H
