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
