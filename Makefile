# Copyright 2020 Grama Nicolae

.PHONY: gitignore clean memory beauty run
.SILENT: beauty clean memory gitignore

# Compilation variables
CC = g++
CFLAGS = -Wno-unknown-pragmas -Wno-unused-parameter -Wall -Wextra -pedantic -g -O3 -std=c++17
INCLUDE = src

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

HOST = ec2-3-8-116-10.eu-west-2.compute.amazonaws.com
PORT = 8080

# Compiles the program
build: $(OBJ)
	@echo "Compiling code..."
	@$(CC) -I$(INCLUDE) -o restcpp ./src/Client.o $(CFLAGS) 
	-@rm -f $(OBJ)

# Runs the server
run: clean build
	@echo "Starting client"
	@./restcpp $(HOST) $(PORT)

%.o: %.cpp
	@$(CC) -I$(INCLUDE) -o $@ -c $< $(CFLAGS) 

# Deletes the binary and object files
clean:
	rm -f restcpp $(OBJ) RestCpp.zip
	echo "Deleted the binary and object files"

# Automatic coding style, in my personal style
beauty:
	clang-format -i -style=file src/*.cpp
	clang-format -i -style=file src/*.hpp

# Checks the memory for leaks
MFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
memory:clean build
	valgrind $(MFLAGS) ./restcpp $(HOST) $(PORT)

# Adds and updates gitignore rules
gitignore:
	@echo "src/*.o" >> .gitignore ||:
	@echo ".vscode*" >> .gitignore ||:
	@echo "restcpp" >> .gitignore ||:	
	echo "Updated .gitignore"
	
# Creates an archive of the project
pack: clean
	zip -FSr RestCpp.zip *

# Git repository statistics (line count)
statistics:
	@git ls-files -x src/* | xargs wc -l | sort -rn

