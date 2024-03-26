Jacson - Json parser library in C


This project is under heavy development and everything might change in the furure.


Architecture:

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
