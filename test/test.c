#include <stdio.h>
#include "../src/jacson.h"


int main(void) {
    char *jdata = "  \"Hello From Json!\"  ";
    (void)jcsn_parse_json(jdata);
    return 0;
}
