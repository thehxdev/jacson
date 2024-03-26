/**
 * Ignore this file for now...
 */


#ifndef JACSON_JACSON_H
#define JACSON_JACSON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#define JACSON_VERSION_MAJOR 0
#define JACSON_VERSION_MINOR 0
#define JACSON_VERSION_PATCH 1
#define JACSON_VERSION "0.0.1"

typedef struct Jcsn_AST Jcsn_AST;

// FIXME: Private types:



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
    char *string;
    byte boolean;
    long integer; // Handle all numbers as integers for now...

    // Don't handle floating point numbers yet...
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

    // Json Objcet situation
    // If this flag is set to 1, indicates that a `name` in json object
    // is waiting for it's value to be set. Otherwise parser can add a
    // new pair or end json object parsing.
    byte situation;
} Jcsn_JObject;


struct Jcsn_AST {
    // Root of AST.
    // It can be either a Json Object or Json Array
    Jcsn_JValue *robj;
    unsigned long depth;
};


typedef struct Jcsn_Parser {
    // Since we're working with pointer arithmic, keep a pointer to
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



// Parse json data from bytes into an AST
Jcsn_AST *jcsn_parse_json(char *jdata);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __JACSON_H__ */