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


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Includes
 */

// Standard library
#include <stdlib.h>
#ifdef __JCSN_TRACE__
    #include <stdio.h>
#endif // __JCSN_TRACE__

// Jacson
#include "types.h"
#include "lexer.h"
#include "str.h"
#include "mem.h"
#include "log.h"



/**
 * Macros and Constants
 */

// Token list default capacity
#define TLIST_DEF_CAP 30



/**
 * Types
 */

typedef struct Jcsn_Tokenizer {
    // Since we're working with pointer arithmic, keep a pointer to
    // first character in json data to prevent out of bound access
    // to memory locations if parser wants to go backward.
    char *first;

    // Pointer to base character in raw json data
    // (Used to extract tokens (sub-strings) with `curr` field)
    char *base;


    // Pointer to current character in raw json data
    char *curr;
} Jcsn_Tokenizer;


typedef struct Jcsn_JNumber {
    union {
        long integer;
        double real;
    } value;

    enum Jcsn_Token_T type;
} __attribute__((packed)) Jcsn_JNumber;



/**
 * Module Private API
 */

// Construct a new token list
static Jcsn_TList *jcsn_tlist_new(size_t cap) {
    Jcsn_TList *tlist = malloc(sizeof(*tlist));
    if (tlist == NULL) {
        JCSN_LOG_ERR("Failed to allocate memory for new token list\n", NULL);
        return NULL;
    }

    *tlist = (Jcsn_TList) {
        .tokens = NULL,
        .cap = cap,
        .len = 0,
    };

    return tlist;
};


// Construct a new raw token
static inline __attribute__((always_inline)) Jcsn_Token
jcsn_token_new(enum Jcsn_Token_T type) {
    return (Jcsn_Token) {
        .type = type,
    };
}


// Free a token from memory
static void jcsn_token_free(Jcsn_Token *t, byte free_strings) {
    if (t) {
        if (t->type == TK_STRING && free_strings)
            xfree(t->value.string);
        // xfree(t);
    }
}


static byte jcsn_tlist_append(Jcsn_TList *tlist, Jcsn_Token *token) {
    if ((tlist->len % tlist->cap) == 0) {
        size_t new_size = (tlist->len + tlist->cap) * sizeof(*tlist->tokens);
        tlist->tokens = realloc(tlist->tokens, new_size);
        if (tlist->tokens == NULL) {
            JCSN_LOG_ERR("Failed to reallocate memory for token list\n", NULL);
            return 1;
        }
    }

    tlist->tokens[tlist->len] = *token;
    tlist->len += 1;
    return 0;
}


// extract a string in between two quotes
static char *jcsn_parse_jstr(char **base, char **curr) {
    char *str = NULL;

    // skip the `"` character
    *curr = (*base += 1);

    while (**curr && **curr != '\"') {
        *curr += 1;
        // skip `"` scape sequance
        if (**curr == '\\' && *(*curr+1) == '\"')
            *curr += 2;
    }

    if (**curr == '\0')
        return NULL;

    str = jcsn_substr_ptr(*base, *curr);
#ifdef __JCSN_TRACE__
    if (str == NULL) {
        JCSN_LOG_ERR("jcsn_substr_ptr returned NULL\n", NULL);
        JCSN_LOG_ERR("Failed to parse a json string\n", NULL);
        JCSN_LOG_INF("Returning NULL\n", NULL);
    }
#endif // __JCSN_TRACE__

    // put *base after last `"` character
    *base = (*curr += 1);
    return str;
}


// parse a number in json data to it's actual value
static Jcsn_JNumber jcsn_parse_jnum(char **base, char **curr) {
    char ch, *tmp = NULL;
    byte negative = 0;
    byte has_point = 0;
    Jcsn_JNumber num = {
        .value = { 0 },
        .type = TK_NULL,
    };

    switch (**base) {
        case '-':
            negative = 1;
        case '+':
            *base += 1;
    }

    *curr = (*base + 1);
    while ((ch = **curr)) {
        if (jcsn_is_digit(ch)) {
            *curr += 1;
        }
        else if (ch == '.' && jcsn_is_digit(*(*curr += 1))) {
            if (has_point) // invalid number
                return num;
            has_point = 1;
        }
        else
            break;
    }

    tmp = jcsn_substr_ptr(*base, *curr);
    if (has_point) {
        num.value.real = (negative) ? (atof(tmp) * -1) : atof(tmp);
        num.type = TK_DOUBLE;
    } else {
        num.value.integer = (negative) ? (atol(tmp) * -1) : atol(tmp);
        num.type = TK_INTEGER;
    }

    *base = *curr;
    xfree(tmp);
    return num;
}



/**
 * Module Public API
 */

// Free all memory allocated by a token list
void jcsn_tlist_free(Jcsn_TList **tlist, byte free_strings) {
    Jcsn_TList *tl = *tlist;
    if (tl) {
        for (size_t i = 0; i < tl->len; i++)
            jcsn_token_free(&tl->tokens[i], free_strings);
        xfree(tl->tokens);
        xfree(tl);
    }
}


Jcsn_TList *jcsn_tokenize_json(char *jdata) {
    Jcsn_Tokenizer tokenizer = {
        .first = jdata,
        .base  = jdata,
        .curr  = jdata,
    };

    Jcsn_TList *tlist = jcsn_tlist_new(TLIST_DEF_CAP);
    if (tlist == NULL) {
        JCSN_LOG_ERR("Failed to construct a new token list\n", NULL);
        JCSN_LOG_ERR("Failed to parse json data\n", NULL);
        return NULL;
    }

    char ch, *tmp;
    Jcsn_JNumber num = { {0}, TK_NULL };
    Jcsn_Token tk = { 0 };
    enum Jcsn_Token_T tk_type;

    jcsn_skip_whitespaces(&tokenizer.base);
    while ((ch = *tokenizer.base)) {

        if (ch == '{') {
            tk_type = TK_OBJ_BEG;
            tokenizer.base += 1;
        }

        else if (ch == '}') {
            tk_type = TK_OBJ_END;
            tokenizer.base += 1;
        }

        else if (ch == '[') {
            tk_type = TK_ARR_BEG;
            tokenizer.base += 1;
        }

        else if (ch == ']') {
            tk_type = TK_ARR_END;
            tokenizer.base += 1;
        }

        else if (ch == ',') {
            tk_type = TK_COMMA;
            tokenizer.base += 1;
        }

        else if (ch == ':') {
            tk_type = TK_SET;
            tokenizer.base += 1;
        }

        else if (ch == '\"') {
            tk_type = TK_STRING;
            tk = jcsn_token_new(tk_type);
            tk.value.string = jcsn_parse_jstr(&tokenizer.base, &tokenizer.curr);
            if (tk.value.string == NULL) {
                JCSN_LOG_ERR("Failed to parse json string\n", NULL);
                JCSN_LOG_ERR("Check json data syntax for errors\n", NULL);
                jcsn_token_free(&tk, true);
                jcsn_tlist_free(&tlist, true);
                return NULL;
            }
            goto append;
        } // end tokenize json string

        else if (ch == 'n') {
            tk_type = TK_NULL;
            if ((tmp = jcsn_str_exact_start(tokenizer.base, "null"))) {
                tk = jcsn_token_new(tk_type);
                tokenizer.base = tmp;
                goto append;
            } else {
                JCSN_LOG_ERR("Invalid token while parsing json null\n", NULL);
                JCSN_LOG_ERR("Token does not match with \'null\'\n", NULL);
                jcsn_tlist_free(&tlist, true);
                return NULL;
            }
        } // end tokenize json null

        else if (ch == 't' || ch == 'f') {
            tk_type = TK_BOOL;
            tk = jcsn_token_new(tk_type);
            if ((tmp = jcsn_str_exact_start(tokenizer.base, "true"))) {
                tk.value.boolean = true;
                tokenizer.base = tmp;
            }
            else if ((tmp = jcsn_str_exact_start(tokenizer.base, "false"))) {
                tk.value.boolean = false;
                tokenizer.base = tmp;
            }
            else {
                JCSN_LOG_ERR("Invalid token while parsing json boolean value\n", NULL);
                JCSN_LOG_ERR("Token does not match with \'true\' or \'false\'\n", NULL);
                jcsn_token_free(&tk, true);
                jcsn_tlist_free(&tlist, true);
                return NULL;
            }
            goto append;
        } // end tokenize json boolean

        else if (ch == '+' || ch == '-' || jcsn_is_digit(ch)) {
            num = jcsn_parse_jnum(&tokenizer.base, &tokenizer.curr);
            tk_type = num.type;
            switch (num.type) {
                case TK_INTEGER:
                    tk = jcsn_token_new(tk_type);
                    tk.value.integer = num.value.integer;
                    break;

                case TK_DOUBLE:
                    tk = jcsn_token_new(tk_type);
                    tk.value.real = num.value.real;
                    break;

                default: {
                    JCSN_LOG_ERR("Invalid token while parsing json number value\n", NULL);
                    JCSN_LOG_ERR("Token does not match with a valid number\n", NULL);
                    jcsn_tlist_free(&tlist, true);
                    return NULL;
                }
            } // end switch(num.type)
            goto append;
        } // end tokenize json number

        else {
            JCSN_LOG_ERR("Invalid character while parsing json data: %c (ascii: %d)\n", ch, ch);
            jcsn_tlist_free(&tlist, true);
            return NULL;
        }

        tk = jcsn_token_new(tk_type);
append:
        jcsn_tlist_append(tlist, &tk);
        jcsn_skip_whitespaces(&tokenizer.base);
    } // end while loop

    return tlist;
}


Jcsn_Token *jcsn_tlist_get(Jcsn_Token *tks, size_t len, long idx) {
    return ((idx >= 0) && (idx < (long)len)) ? &tks[idx] : NULL;
}

#ifdef __cplusplus
}
#endif // __cplusplus
