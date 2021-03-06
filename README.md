# sudoku_game_cpp
## Generic Sudoku Game
- Run in terminal 
- Developed in C++11
- Support different types of Sudoku
- Level evaluation
- Game generation with given level
- Load/Write from/to files

## Run

#### To run on Linux
```bash
bin/sudoku_Linux
```

#### To run on MacOS
```bash
bin/sudoku_Darwin
```

## Build

#### To build release version
```bash
make
```
#### To build debug version
```bash
make sudoku_debug
```

#### To build static-linking version
```bash
make sudoku_static
```

#### To build for diagnose
```bash
make sudoku_diagnose
```

## Test

#### To run simple testing
```bash
make sudoku_testing
```

#### To run google unit testing
```bash
make sudoku_google_testing
```
> Due to C++ incompatible ABI, Unit testing may not be supported on MacOS yet. Please run it with Linux(Ubuntu or Centos)


## Note

- Binaries are going to be generated under bin folder.
- Bunch of board files are under bin folder.
- You may need access to write a game board to a file.
