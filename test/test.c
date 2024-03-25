#include <stdio.h>
#include <assert.h>
#include "../src/jacson.h"


int main(void) {
    size_t i;
    char *jdata = "[ [ \"nested array string\" ], \"This is a string inside json array\", \"Another string\" ]";
    Jcsn_AST *ast = jcsn_parse_json(jdata);

    Jcsn_JValue *root = ast->robj;

    assert(root->type == J_ARRAY);

    printf("\n");
    for (i = 1; i < root->data.array->len; i++)
        printf("%s\n", root->data.array->vals[i]->data.string);

    Jcsn_JValue *nested_arr = ast->robj->data.array->vals[0];
    for (i = 0; i < nested_arr->data.array->len; i++)
        printf("%s\n", nested_arr->data.array->vals[i]->data.string);

    return 0;
}

