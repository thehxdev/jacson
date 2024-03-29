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


#ifndef JACSON_JACSON_H
#define JACSON_JACSON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**
 * Macros and constants
 */

#define JACSON_VERSION_MAJOR 0
#define JACSON_VERSION_MINOR 0
#define JACSON_VERSION_PATCH 1
#define JACSON_VERSION "0.0.1"

#ifndef byte
    #define byte char
#endif // byte



/**
 * Types
 */

typedef struct Jcsn_AST Jcsn_AST;


// FIXME: Implement an interface for interacting with AST
// and make these types private.

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

union __Jcsn_JValue {
    struct Jcsn_JArray *array;
    struct Jcsn_JObject *object;
    char *string;
    byte boolean;
    long integer;
    double real; // Fortran programmer vibe, huh? :)
};

typedef struct Jcsn_JValue {
    union __Jcsn_JValue data;
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
    Jcsn_JValue *root;
    unsigned long depth;
};



/**
 * Module Public API
 */

// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parse_json(char *jdata);

// Print the parsed AST
void jcsn_ast_print(Jcsn_AST *ast);

// Free all memory used by ast
void jcsn_ast_free(Jcsn_AST **ast);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __JACSON_H__ */
