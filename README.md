# Jacson
Jacson is a simple Json parsing library in C that builds an AST from raw json data.

> [!WARNING]
> This project is under development and everything might change in the future.


## Build
To build Jacson, you need a C compiler and `cmake` installed on your system.

```bash
git clone --depth=1 --branch=main https://github.com/thehxdev/jacson

cd jacson

mkdir -p build && cmake -B build/ -S .

cmake --build build
```

Then you can use `libjacson.a` file in `build` directory. Or use `test` program in `build` directory to parse a json file and query data from it.


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
- [x] Interface to interact with AST (Getting/Changing data)
- [ ] Better and more advanced query engine
- [ ] Handle scape sequences in string data
- [ ] Handle scape sequences in query strings
- [ ] Documentation for Jacson's public API
- [ ] Error handling and reporting errors to top-level callers
- [ ] More advanced json syntax validation
- [ ] Lazy evaluation capabilities
- [ ] Write tests for each module
