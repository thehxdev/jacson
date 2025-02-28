/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Validator Module
 * Validate tokens.
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef __JACSON_VALIDATOR_H
#define __JACSON_VALIDATOR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


int jcsn_validate_tokens(Jcsn_TList *tlist);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_VALIDATOR_H
