/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Lexical Analysis Module
 * Convert raw json data into a list of tokens
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef __JACSON_LEXER_H
#define __JACSON_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Types
 */

// type of json tokens
enum Jcsn_Token_T {
    // Single character tokens
    TK_OBJ_BEG = '{',
    TK_OBJ_END = '}',
    TK_ARR_BEG = '[',
    TK_ARR_END = ']',
    TK_SET      = ':',
    TK_COMMA    = ',',

    // Json values
    TK_STRING,
    TK_INTEGER,
    TK_DOUBLE,
    TK_BOOL,
    TK_NULL,
} __attribute__((packed));


// a Json Token
typedef struct Jcsn_Token {
    union {
        char   *string;
        byte   boolean;
        long   integer;
        double real;
    } value;
    enum Jcsn_Token_T type;
} __attribute__((packed)) Jcsn_Token;


// a dynamic array to store tokens
typedef struct Jcsn_TList {
    Jcsn_Token **tokens;
    unsigned long len;
    unsigned long cap;
} __attribute__((packed)) Jcsn_TList;


Jcsn_TList *jcsn_tokenize_json(char *jdata);

void jcsn_tlist_free(Jcsn_TList **tlist, byte free_strings);

// get a token from token list
Jcsn_Token *jcsn_tlist_get(Jcsn_Token **tks, unsigned long len, long idx);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_LEXER_H

