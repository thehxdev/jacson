# Jacson
Jacson is a Json parser and processing library in C.

**If you are reading the source code, ignore `jacson.h` and `jacson.c` files for now.**

> [!WARNING]
> This project is under heavy development and everything might change in the furure.


## Architecture:
Jacson architecture

```
    +------------------+
    |                  |
    |   Raw Json Data  |
    |                  |
    +---------+--------+
              |         
              |         
    +---------v--------+
    |                  |
    |     Tokenizer    |
    |                  |
    +---------+--------+
              |         
              |         
    +---------v--------+
    |                  |
    |     Validator    |
    |                  |
    +---------+--------+
              |         
              |         
    +---------v--------+
    |                  |
    |      Parser      |
    |                  |
    +------------------+
```
