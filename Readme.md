# C++ REST API - [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This is a HTTP client application that interacts with a REST API, using different types of HTTP requests. This is the 3nd Communication Protocols Course homework. The problem statement / application basic design is described [here (Romanian)](./docs/problem_statement.pdf).


## Project structure

- src/
  - Client - manages the connections and the input
  - Request - used to create different types of http/1.1 requests
  - Response - used to parse http/1.1 responses, to extract things like status codes, cookies, jwt tokens, etc.
  - Utils - this header is included in all other files, as it contains different macros, functions, data-types, and it includes most of the libraries that are used by the other files.
- docs/ - in this folder are stored different documentation files
- lib/ - contains additional libraries used by the project. Specifically, nlohmann/json
- .clang-format - my personal coding style ruleset. A variation of the google file
- Makefile - a lot of rules used to compile, run, test, etc.


## Application overview

After the client is started, it will try to establish a connection to the server. If operations is successfull, it will be able to process commands from STDIN. 

- register - create a new account. If the `HIDE_PASS` option is set to true, the password must be entered twice (as a safety measure). NOTE : when the password is read, terminal commands are disabled (like ctrl+c). Also, input can't be redirected to the terminal.
- login - login into a existing account. `HIDE_PASS` option affects this operation also, but it only hides the password (as misstyping the password isn't such a big problem).
- enter_library - enter the user's library
- get_books - returns a list with all the user's books (their id and title)
- get_book - after the book id is entered, it will try to return all the information about that book. The id must be a positive( > 0) integer(it will ask for it untill the input is valid)
- add_book - add a new book to the library. The number of pages must also be a positive integer
- remove_book - remove a book from the library. Like in the `get_book` command, the book id must be a positive integer
- logout - logout from the account
- exit - logout and exit the program

## Usage and Makefile

To start the client, simply run `make run` in the terminal.

There are many Makefile commands included, some used to build and run the application, some used for testing or development.

### Variables

HOST - the url of the server
PORT - the port on which the server listens (and the clients will connect to)

### Commands

- build - compiles the application
- run - recompiles the application and starts it
- clean - remove unnecessary files
- beauty - uses clang-format and the file included in this project to "beautify" the code (coding style)
- memory - runs valgrind to check for errors and memory leaks
- pack - creates the "homework submission" archive
- gitignore - creates the gitignore file (and adds rules)
- statistics - shows the total number of lines written for the project and each individual file (in /src and /test) - development command

The application was developed on a Ubuntu 18.04 LTS machine. It was tested using `gcc 7.5.0`, `clang-format 6.0.0` and `valgrind-3.13.0`.

## Other information

- Some of request/response parsing wasn't done as generical as I intended, due to time contraints. Most of the code used for that operations is ported from the laboratory, which was written in C (implemented using c++ stl).
- Some of the input/output is formatted a bit differently from the "problem statement" specifications, because I wanted to make it easier to read the information.

© 2020 Grama Nicolae
