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


#ifndef JACSON_STR_H
#define JACSON_STR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



byte jcsn_is_whitespace(const char ch);

byte jcsn_is_digit(const char ch);

void jcsn_skip_whitespaces(char **ptr);

// `source` string exactly starts with `query`.
char *jcsn_str_exact_start(const char *source, const char *query);

// Get a sub-string between 2 pointers (start and end).
// The returned character pointer is heap allocated.
char *jcsn_substr_ptr(const char *start, const char *end);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_STR_H
