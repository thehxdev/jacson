/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Tracing Module
 * Log important messages for debugging.
 * ------------------------------------------------------------ *
 *
 * Jacson is developed under MIT License. You can find a copy
 * of license information in the project's github repository:
 * https://github.com/thehxdev/jacson/blob/main/LICENSE
 */

#ifndef JACSON_LOG_H
#define JACSON_LOG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



/**
 * Macros
 */

#ifdef __JCSN_TRACE__
    #define JCSN_LOG_ERR(format, ...) \
        (void) fprintf(stderr, "[Jacson - ERROR] %s: " format, __FUNCTION__, __VA_ARGS__)

    #define JCSN_LOG_INF(format, ...) \
        (void) fprintf(stderr, "[Jacson - INFO] %s: " format, __FUNCTION__, __VA_ARGS__)
#else
    #define JCSN_LOG_ERR(format, ...)
    #define JCSN_LOG_INF(format, ...)
#endif // __JCSN_LOG__



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // JACSON_LOG_H
