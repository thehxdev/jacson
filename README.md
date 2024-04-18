# Jacson
Jacson is a simple Json parsing library and query engine in C (in ~1100 LoC).

> [!WARNING]
> This project is just a simple json parser with simple query engine. Don't use this library for real programs.



## Features

- By not using recursion Jacson can handle deeply nested structures.
- Simple Public API



## Build
To build Jacson, you need a C compiler (`gcc` or `clang`), `cmake` and `make` installed on your system.

```bash
git clone --depth=1 --branch=main https://github.com/thehxdev/jacson

cd jacson

mkdir -p build && cmake -B build -S .

cmake --build build
```

Then you can use `libjacson.a` file for your projects in `build` directory and header files in `include` directory.
Or use `test` program in `build` directory to parse a json file and query data from that.

Take a look at `test/test.c` file to see how to use Jacson as a library.



## Query Syntax

Jacson has very simple query syntax to get data from AST (Parsed json data).

- Use `.` to seperate different parts of query.
- Use `[N]` syntax to show an array's index where N is a positive integer or 0.

### Example
Consider this json data:
```json
{
    "message": "Hello World!",
    "status": 200,
    "ok": true,
    "arr": [
        true,
        false,
        56,
        12.841,
        {
            "name": "thehxdev"
        },
        null
    ]
}
```
To get `thehxdev` string, you can use `arr.[4].name` query string.

> [!NOTE]
> Use `test` program in `build` directory to parse and query json files. Execute it with no arguments to get a help message.



## Architecture:
Jacson architecture

```
+------------------+
|   Raw Json Data  |
+---------+--------+
          |         
+---------v--------+
|     Tokenizer    |
+---------+--------+
          |         
+---------v--------+
|     Validator    |
+---------+--------+
          |         
+---------v--------+
|      Parser      |
+------------------+
```


## TODO

List of improvements and features to add:
- [x] Free all memory used by Jacson (without recursion)
- [x] Query engine for getting data from AST
- [ ] Change or add data to AST
- [ ] Better and more advanced query engine
- [ ] Handle scape sequences in string data
- [ ] Handle scape sequences in query strings
- [ ] Documentation for Jacson's public API
- [ ] Error handling and reporting errors to top-level callers
- [ ] More advanced json syntax validation
- [ ] Lazy evaluation capabilities
- [ ] Write tests for each module
