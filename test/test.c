#include <stdio.h>
#include <assert.h>
#include "../src/jacson.h"


int main(void) {
    size_t i;
    char *jdata = "{ \"test\": [ \"hi\", \"Hello\" ], \"name\": \"Hossein\", \"age\": \"20\", \"city\": null }";
    Jcsn_AST *ast = jcsn_parse_json(jdata);

    Jcsn_JValue *root = ast->robj;

    assert(root->type == J_OBJECT);

    printf("\n");

    Jcsn_JArray *arr = root->data.object->values[0]->data.array;
    for (i = 0; i < arr->len; i++)
        printf("%s\n", arr->vals[i]->data.string);


    for (i = 1; i < root->data.object->len - 1; i++) {
        printf("%s : ", root->data.object->names[i]);
        printf("%s\n", root->data.object->values[i]->data.string);
    }

    assert(root->data.object->values[i+1] == NULL);

    return 0;
}

