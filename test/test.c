#include <stdio.h>
#include <stdlib.h>
#include <jacson/jacson.h>

char *read_file(const char *path);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <json-file-path>\n", argv[0]);
        return 1;
    }

    char *jdata = read_file(argv[1]);
    Jacson *j = jcsn_parse_json(jdata);


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

    content = malloc(sizeof(char) * size);
    fread(content, sizeof(char), size, fp);

    fclose(fp);
    return content;
}
