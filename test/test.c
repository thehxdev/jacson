#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <jacson/jacson.h>

#ifdef __linux__
    #include <unistd.h>
    #include <sys/mman.h>
#endif

char *read_file(const char *path);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <json-file-path> <query>\n", argv[0]);
        return 1;
    }

    char *jdata = read_file(argv[1]);
    assert(jdata != NULL && "read_file returned NULL");

    Jacson *j = jcsn_parse_json(jdata);
    assert(j != NULL && "jcsn_parse_json returned NULL");

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
    jcsn_free(j);
    return 0;
}


char *read_file(const char *path) {
    struct stat sb;
    char *content;

    FILE *fp = fopen(path, "r");
    assert(fp != NULL && "failed to open input file");

    if (fstat(fileno(fp), &sb) == -1)
        return NULL;
#ifdef __linux__
    content = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
#else
    content = calloc(sb.st_size + 1, sizeof(char));
    if (!content) {
        goto ret;
    }
    (void)fread(content, sizeof(char), sb.st_size, fp);
ret:
#endif
    fclose(fp);
    return content;
}
