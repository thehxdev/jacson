/**
 * Jacson
 *
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Description: Json processing library in C.
 * Git: https://github.com/thehxdev/jacson
 *
 * ------------------------------------------------------------ *
 * Query Module
 * Query data from AST
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

// Standard Library
#ifdef __JCSN_TRACE__
    #include <stdio.h>
#endif // __JCSN_TRACE__
#include <stdlib.h>
#include <string.h>

// Jacson
#include "log.h"
#include "str.h"
#include "mem.h"
#include "query.h"


/**
 * Types
 */

// Type of each token in the query string
enum Jcsn_QType {
    // Q_ROOT, // root of AST
    Q_NAME, // a name in json object
    Q_IDX,  // an index in json array
} __attribute__((packed));


// a query token
typedef struct jcsn_qtoken {
    union {
        char *str;
        long idx;
    } data;
    enum Jcsn_QType type;
} __attribute__((packed)) Jcsn_QToken;


// an array of tokens
typedef struct jcsn_qtlist {
    Jcsn_QToken *tokens;
    size_t len;
} __attribute__((packed)) Jcsn_QTList;


/**
 * Module Private API
 */

// Initialize token list
static void jcsn_qtlist_init(Jcsn_QTList *tlist,
                             const char *query,
                             const size_t q_len)
{
    size_t i;
    for (i = 0; i < q_len; i++)
        if (query[i] == '.')
            tlist->len += 1;
    // Include last token
    tlist->len += 1;

    tlist->tokens = malloc(sizeof(Jcsn_QToken) * tlist->len);
    if (!tlist->tokens) {
        JCSN_LOG_ERR("Failed to allocate memory for query token list\n", NULL);
        tlist->len = 0;
    }
}


static Jcsn_QTList jcsn_tokenize_query(char *query) {
    Jcsn_QTList tlist = {
        .tokens = NULL,
        .len = 0,
    };

    size_t q_len = strlen(query);
    if (q_len == 0)
        goto ret;

    jcsn_qtlist_init(&tlist, query, q_len);
    if (!tlist.tokens)
        goto ret;

    long idx, i = 0;
    char *tk_str = NULL, *s_ptr = NULL, *q = strdup(query);
    Jcsn_QToken token = { 0 };

    tk_str = strtok_r(q, ".", &s_ptr);
    while (tk_str) {
        if (*tk_str == '[') {
            idx = jcsn_parse_long(tk_str);
            token = (Jcsn_QToken) {
                .type = Q_IDX,
                .data.idx = idx,
            };
        } else {
            token = (Jcsn_QToken) {
                .type = Q_NAME,
                .data.str = strdup(tk_str),
            };
        }

        tk_str = strtok_r(NULL, ".", &s_ptr);
        tlist.tokens[i] = token;
        i += 1;
    }

    free(q);
ret:
    return tlist;
}


static Jcsn_JValue *jcsn_collection_find(Jcsn_JValue *coll, Jcsn_QToken *tk) {
    size_t i = 0;
    Jcsn_JValue *res = NULL;
    switch (coll->type) {
        case J_ARRAY: {
            if (tk->type != Q_IDX)
                return NULL;
            return coll->data.array->vals[tk->data.idx];
        }
        break;

        case J_OBJECT: {
            if (tk->type != Q_NAME)
                return NULL;
            Jcsn_JObject *obj = coll->data.object;
            for (i = 0; i < obj->len; i++) {
                if (strcmp(tk->data.str, obj->names[i]) == 0)
                    return obj->values[i];
            }
        }
        break;

        default:
            return NULL;
    }

    return res;
}


static void jcsn_qtlist_free(Jcsn_QTList *qtl) {
    if (qtl) {
        for (size_t i = 0; i < qtl->len; i++) {
            Jcsn_QToken *t = &qtl->tokens[i];
            if (t->type == Q_NAME)
                xfree(t->data.str);
        }
        xfree(qtl->tokens);
    }
}



/**
 * Module Public API
 */

// Get a value from AST
Jcsn_JValue *jcsn_query_value(Jcsn_JValue *root, char *query) {
    Jcsn_QToken t = { 0 };
    Jcsn_JValue *result = NULL, *scope = root;

    Jcsn_QTList tlist = jcsn_tokenize_query(query);
    if (tlist.len == 0) {
        JCSN_LOG_ERR("Token list is empty\n", NULL);
        return NULL;
    }

    for (size_t i = 0; i < tlist.len; i++) {
        t = tlist.tokens[i];
        if (scope->type == J_ARRAY && t.type != Q_IDX) {
            JCSN_LOG_ERR("Could not query a json array with a Q_NAME token\n", NULL);
            jcsn_qtlist_free(&tlist);
            return NULL;
        }

        result = jcsn_collection_find(scope, &t);
        if (!result)
            break;

        if (result->type == J_ARRAY || result->type == J_OBJECT)
            scope = result;
    }

    jcsn_qtlist_free(&tlist);
    return result;
}



#ifdef __cplusplus
}
#endif // __cplusplus
