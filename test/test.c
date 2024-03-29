#include <stdio.h>
#include <jacson/jacson.h>


int main(void) {
    char *jdata = "{ \"arr\": [ { \"msg\": \"Hello From AST!\" } ], \"status\": 200, \"ok\": true, \"float_num\": 224.123, \"neg\": -90 }";
    Jcsn_AST *ast = jcsn_parse_json(jdata);
    if (!ast)
        return 1;

    Jcsn_JValue *root = ast->root;
    printf("AST Depth = %zu\n\n", ast->depth);

    Jcsn_JValue *arr = root->data.object->values[0];
    Jcsn_JValue *inner_obj = arr->data.array->vals[0];
    printf("%s: %s\n",
           inner_obj->data.object->names[0],
           inner_obj->data.object->values[0]->data.string);

    printf("%s: %ld\n",
           root->data.object->names[1],
           root->data.object->values[1]->data.integer);

    // Not implemented yet
    jcsn_ast_free(&ast);
    return 0;
}

