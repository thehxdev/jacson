#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <jacson/jacson.h>

char *read_file(const char *path);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <json-file-path> <query>\n", argv[0]);
        return 1;
    }

    struct timespec start, end;

    char *jdata = read_file(argv[1]);

    clock_gettime(CLOCK_REALTIME, &start);
    Jacson *j = jcsn_parse_json(jdata);
    clock_gettime(CLOCK_REALTIME, &end);

    printf("parsing took %lu nano secs\n", end.tv_nsec - start.tv_nsec);

    Jcsn_JValue *root = jcsn_ast_root(j);
    if (!root) {
        fprintf(stderr, "AST root is NULL\n");
        goto ret;
    }

    clock_gettime(CLOCK_REALTIME, &start);
    Jcsn_JValue *result = jcsn_get_value(j, argv[2]);
    clock_gettime(CLOCK_REALTIME, &end);
    printf("getting value took %lu nano secs\n\n", end.tv_nsec - start.tv_nsec);

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
    if (!fp) {
        perror("Filed to open file");
        exit(1);
    }

    if (fstat(fileno(fp), &sb) == -1)
        return NULL;

    content = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
    fclose(fp);
    return content;
}
