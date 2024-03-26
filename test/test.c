#include <stdio.h>
#include "../src/types.h"
#include "../src/lexer.h"


int main(void) {
    char *jdata = "{ \"msg\": \"Hello World!\", \"status\": 200, \"ok\": true, \"float_num\": 224.123, \"neg\": -90 }";
    Jcsn_Token *t = NULL;
    Jcsn_TList *tks = jcsn_tokenize_json(jdata);
    for (size_t i = 0; i < tks->len; i++) {
        t = tks->tokens[i];
        printf("token = %p -> ", t);
        switch (t->type) {
            case TK_INTEGER:
                printf("integer = %ld\n", t->value.integer);
                break;

            case TK_DOUBLE:
                printf("double = %.3lf\n", t->value.real);
                break;

            case TK_STRING:
                printf("string = %s\n", t->value.string);
                break;

            case TK_BOOL:
                printf("boolean = %d\n", t->value.boolean);
                break;

            case TK_NULL:
                printf("null\n");
                break;

            case TK_OBJ_BEG:
                printf("{\n");
                break;

            case TK_OBJ_END:
                printf("}\n");
                break;

            case TK_ARR_BEG:
                printf("[\n");
                break;

            case TK_ARR_END:
                printf("]\n");
                break;

            case TK_SET:
                printf(":\n");
                break;

            case TK_COMMA:
                printf(",\n");
                break;
        }
    }
    return 0;
}

