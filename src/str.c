/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * String Manipulation Module
 * General functionalities for working with strings and characters.
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
#include "log.h"
#include "str.h"



/**
 * Module Public API
 */

int jcsn_string_append(Jcsn_String *jstr, const char *s, size_t slen) {
    // include null terminator while allocating more memory
    slen += 1;
    if ((jstr->len % jstr->cap) <= slen) {
        size_t blocks = ((int)((jstr->len + slen) / jstr->cap)) + 1;
        jstr->data = realloc(jstr->data, (sizeof(char) * blocks * jstr->cap));
        if (jstr->data == NULL)
            return 1;
    }

    slen -= 1;
    char *last_ptr = &((jstr->data)[jstr->len]);
    memmove(last_ptr, s, slen * sizeof(char));

    jstr->len += slen;
    jstr->data[jstr->len] = '\0';

    return 0;
}


void jcsn_string_clear(Jcsn_String *jstr) {
    jstr->len = 0;
    free(jstr->data);
    jstr->data = NULL;
}


// `source` string exactly starts with `query`
char *jcsn_str_exact_start(const char *source, const char *query) {
    // example: s = "Hello World!", q = "Hello" -> True
    if (source == NULL) {
        JCSN_LOG_ERR("source string is null\n", NULL);
        return NULL;
    }

    if (query == NULL) {
        JCSN_LOG_ERR("query string is null\n", NULL);
        return NULL;
    }

    register char *s = (char*)source;
    register char *q = (char*)query;

    if (*s != *q)
        return NULL;

    while (*s && *q) {
        if (*s == *q) {
            s += 1;
            q  += 1;
        }
        else
            break;
    }

    return (*q == '\0') ? s : NULL;
}


char *jcsn_substr_ptr(const char *start, const char *end) {
    // For example extracting a string in between two quotes:
    //   "a json string example"
    //    ^                    ^
    //    *start               *end
    //    |--------------------|
    //            delta

    char *str = NULL;
    size_t delta = 0;

    if (end < start) {
        JCSN_LOG_INF("Ending address (%p) is lower than starting address (%p)\n", end, start);
        JCSN_LOG_INF("Returning NULL\n", NULL);
        goto ret;
    }

    delta = end - start;
    str = calloc(delta + 1, sizeof(char));
    if (str == NULL) {
        JCSN_LOG_ERR("Failed to allocate memory for new string\n", NULL);
        JCSN_LOG_INF("Returning NULL\n", NULL);
        goto ret;
    }

    str = strncpy(str, start, delta);

#ifdef __JCSN_TRACE__
    if (str == NULL) {
        JCSN_LOG_ERR("strncpy function failed\n", NULL);
        JCSN_LOG_ERR("Failed to copy source string to destination", NULL);
        JCSN_LOG_INF("Returning NULL\n", NULL);
    }
#endif // __JCSN_TRACE__

ret:
    return str;
}


long jcsn_parse_long(const char *s) {
    char *tmp = (char*)s;
    long num = -1;
    short neg = 0;

    while (*tmp && !(jcsn_is_digit(*tmp))) tmp++;
    if (*tmp == '\0') {
        JCSN_LOG_INF("given string does not contain any numbers\n", NULL);
        goto ret;
    }

    if (tmp != s && *(tmp-1) == '-')
        neg = 1;

    num = 0;
    while (*tmp && jcsn_is_digit(*tmp)) {
        num *= 10;
        num += (*tmp - 48);
        tmp += 1;
    }

ret:
    return (neg) ? (num * -1) : num;
}


#ifdef __cplusplus
}
#endif // __cplusplus
