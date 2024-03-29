# Jacson
Jacson is a simple Json parsing library in C that builds an AST from raw json data.

> [!WARNING]
> This project is under development and everything might change in the future.


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
- [ ] Free all memory used by Jacson
- [ ] Handle scape sequences in string data
- [ ] Documentation for Jacson's public API
- [ ] Error handling and reporting errors to top-level callers
- [ ] Interface to interact with AST (Getting/Changing data)
- [ ] More advanced json syntax validation
- [ ] Lazy evaluation capabilities
- [ ] Write tests for each module
