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
#include <string.h>
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

#define TLIST_CAP_GROW_UNIT 30



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
} Jcsn_JNumber;



/**
 * Module Private API
 */

static byte jcsn_tlist_append(Jcsn_TList *tlist, Jcsn_Token *token) {
    if (tlist->len == tlist->cap) {
        tlist->cap += TLIST_CAP_GROW_UNIT;
        size_t new_size = tlist->cap * sizeof(*tlist->tokens);
        tlist->tokens = realloc(tlist->tokens, new_size);
        if (!tlist->tokens) {
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
    char ch, *sub;
    Jcsn_String str = jcsn_string_new(15);

    // skip first `"` character
    *curr = (*base += 1);

    while (**curr && **curr != '\"') {
        if (**curr == '\\') {
            sub = jcsn_substr_ptr(*base, *curr);
            jcsn_string_append(&str, sub, strlen(sub));
            xfree(sub);

            *curr += 1;
            switch (**curr) {
                case '\"':
                    ch = '\"';
                    break;
                case '\\':
                    ch = '\\';
                    break;
                case '/':
                    ch = '/';
                    break;
                case 'b':
                    ch = '\b';
                    break;
                case 'n':
                    ch = '\n';
                    break;
                case 'r':
                    ch = '\r';
                    break;
                case 't':
                    ch = '\t';
                    break;
                case 'f':
                    ch = '\f';
                    break;
            }
            jcsn_string_append(&str, &ch, 1);
            *base = *curr + 1;
        }
        *curr += 1;
    }

    if (**curr == '\0') {
        xfree(sub);
        xfree(str.data);
        return NULL;
    }

    sub = jcsn_substr_ptr(*base, *curr);
    jcsn_string_append(&str, sub, strlen(sub));

#ifdef __JCSN_TRACE__
    if (str.data == NULL) {
        JCSN_LOG_ERR("jcsn_substr_ptr returned NULL\n", NULL);
        JCSN_LOG_ERR("Failed to parse a json string\n", NULL);
        JCSN_LOG_INF("Returning NULL\n", NULL);
    }
#endif // __JCSN_TRACE__

    // put *base after last `"` character
    *base = (*curr += 1);
    xfree(sub);
    return str.data;
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
void jcsn_tlist_free(Jcsn_TList *tl, byte free_strings) {
    Jcsn_Token t;
    if (tl) {
        for (size_t i = 0; i < tl->len; i++) {
            t = tl->tokens[i];
            if (t.type == TK_STRING && free_strings)
                xfree(t.value.string);
        }
        xfree(tl->tokens);
        tl->len = 0;
        tl->cap = 0;
    }
}


Jcsn_TList jcsn_tokenize_json(char *jdata) {
    Jcsn_Tokenizer tokenizer = {
        .first = jdata,
        .base  = jdata,
        .curr  = jdata,
    };

    char ch, *tmp, err = 0;
    Jcsn_TList tlist = { NULL, 0, 0 };
    Jcsn_JNumber num = { {0}, TK_NULL };
    Jcsn_Token tk = {0};
    enum Jcsn_Token_T tk_type;

    while (1) {
        jcsn_skip_whitespaces(&tokenizer.base);
        ch = *tokenizer.base;
        if (!ch)
            break;

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
            tk = (Jcsn_Token) { .type = tk_type };
            tk.value.string = jcsn_parse_jstr(&tokenizer.base, &tokenizer.curr);
            if (tk.value.string == NULL) {
                JCSN_LOG_ERR("Failed to parse json string\n", NULL);
                JCSN_LOG_ERR("Check json data syntax for errors\n", NULL);
                xfree(tk.value.string);
                err = 1;
                goto ret;
            }
            goto append;
        } // end tokenize json string

        else if (ch == 'n') {
            tk_type = TK_NULL;
            if ((tmp = jcsn_str_exact_start(tokenizer.base, "null"))) {
                tk = (Jcsn_Token) { .type = tk_type };
                tokenizer.base = tmp;
                goto append;
            } else {
                JCSN_LOG_ERR("Invalid token while parsing json null\n", NULL);
                JCSN_LOG_ERR("Token does not match with \'null\'\n", NULL);
                err = 1;
                goto ret;
            }
        } // end tokenize json null

        else if (ch == 't' || ch == 'f') {
            tk_type = TK_BOOL;
            tk = (Jcsn_Token) { .type = tk_type };
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
                err = 1;
                goto ret;
            }
            goto append;
        } // end tokenize json boolean

        else if (ch == '+' || ch == '-' || jcsn_is_digit(ch)) {
            num = jcsn_parse_jnum(&tokenizer.base, &tokenizer.curr);
            tk_type = num.type;
            switch (num.type) {
                case TK_INTEGER:
                    tk = (Jcsn_Token) { .type = tk_type };
                    tk.value.integer = num.value.integer;
                    break;

                case TK_DOUBLE:
                    tk = (Jcsn_Token) { .type = tk_type };
                    tk.value.real = num.value.real;
                    break;

                default: {
                    JCSN_LOG_ERR("Invalid token while parsing json number value\n", NULL);
                    JCSN_LOG_ERR("Token does not match with a valid number\n", NULL);
                    err = 1;
                    goto ret;
                }
            } // end switch(num.type)
            goto append;
        } // end tokenize json number

        else {
            JCSN_LOG_ERR("Invalid character while parsing json data: %c (ascii: %d)\n", ch, ch);
            err = 1;
            goto ret;
        }

        tk = (Jcsn_Token) { .type = tk_type };
append:
        jcsn_tlist_append(&tlist, &tk);
    } // end while loop

ret:
    if (err)
        jcsn_tlist_free(&tlist, true);
    return tlist;
}


Jcsn_Token *jcsn_tlist_get(Jcsn_Token *tks, size_t len, long idx) {
    return ((idx >= 0) && (idx < (long)len)) ? &tks[idx] : NULL;
}

#ifdef __cplusplus
}
#endif // __cplusplus
