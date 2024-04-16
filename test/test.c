#include <stdio.h>
#include <stdlib.h>
#include <jacson/jacson.h>

char *read_file(const char *path);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <json-file-path> <query>\n", argv[0]);
        return 1;
    }

    char *jdata = read_file(argv[1]);
    Jacson *j = jcsn_parse_json(jdata);

    Jcsn_JValue *root = jcsn_ast_root(j);
    if (!root) {
        fprintf(stderr, "AST root is NULL\n");
        goto ret;
    }

    Jcsn_JValue *result = jcsn_get_value(j, argv[2]);
    if (!result) {
        fprintf(stderr, "Query result is NULL\n");
        goto ret;
    }

    printf("%s -> ", argv[2]);
    switch (result->type) {
        case J_OBJECT:
            printf("json object\n");
            break;

        case J_ARRAY:
            printf("json array\n");
            break;

        case J_BOOL:
            printf("%d\n", result->data.boolean);
            break;

        case J_INTEGER:
            printf("%ld\n", result->data.integer);
            break;

        case J_REAL:
            printf("%lf\n", result->data.real);
            break;

        case J_STRING:
            printf("%s\n", result->data.string);
            break;

        case J_NULL:
            printf("null\n");
            break;

    }

ret:
    jcsn_free(&j);
    free(jdata);
    return 0;
}


char *read_file(const char *path) {
    char *content = NULL;
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("Filed to open file");
        exit(1);
    }

    fseek(fp, SEEK_SET, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    content = calloc(size + 1, sizeof(char));
    fread(content, sizeof(char), size, fp);

    fclose(fp);
    return content;
}
