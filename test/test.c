#include <stdio.h>
#include <jacson/jacson.h>


int main(void) {
    char *jdata = "{ \"arr\": [ { \"msg\": \"Hello From AST!\" } ], \"status\": 200, \"ok\": true, \"float_num\": 224.123, \"neg\": -90 }";
    Jacson *j = jcsn_parse_json(jdata);


    jcsn_free(&j);
    return 0;
}

