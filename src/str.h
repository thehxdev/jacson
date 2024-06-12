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


#ifndef __JACSON_STR_H
#define __JACSON_STR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Macros and Constants
 */

#define jcsn_is_whitespace(ch) \
    ((ch) == ' ' || (ch) == '\t' || (ch) == '\n' || ch == '\r')


#define jcsn_skip_whitespaces(ptr) \
    while ((**(ptr)) && ((jcsn_is_whitespace(**(ptr))))) *(ptr) += 1


#define jcsn_is_digit(ch) \
    (((ch) >= '0') && ((ch) <= '9'))


#define JCSN_STRING_DEF_CAP (25)


#define jcsn_string_new(cap)    \
    (Jcsn_String){ NULL, 0, (((cap) > 0) ? ((cap)) : (JCSN_STRING_DEF_CAP)) }



/**
 * Types
 */

typedef struct Jcsn_String {
    char *data;
    size_t len;
    size_t cap;
} Jcsn_String;



/**
 * Module Public API
 */

// append to a dynamic string
int jcsn_string_append(Jcsn_String *jstr, const char *s, size_t slen);

// clear string buffer
void jcsn_string_clear(Jcsn_String *jstr);

// `source` string exactly starts with `query`.
char *jcsn_str_exact_start(const char *source, const char *query);

// Get a sub-string between 2 pointers (start and end).
// The returned character pointer is heap allocated.
char *jcsn_substr_ptr(const char *start, const char *end);


// Parse a single integer value from string literal
long jcsn_parse_long(const char *s);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_STR_H
