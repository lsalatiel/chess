CC = g++
CFLAGS = -Wall -g
EXEC = main
SRC_DIR = src
OBJ_DIR = obj
CPP = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/chesslib/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP))
CPPFLAGS = -std=c++23

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(EXEC)
