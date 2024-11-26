CC = g++
CFLAGS = -std=c++23 -ggdb -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror
LDFLAGS = -L/usr/local/lib -L/usr/local/lib/yaml-cpp -lfltk_images -lfltk_jpeg /usr/lib/libpng.so /usr/lib/libz.so \
          -lfltk_gl -lGLU -lGL -lfltk -lm -lX11 -lXext -lpthread -lXinerama -lXfixes -lXcursor -lXft -lfontconfig -ldl -lyaml-cpp \
          -lXrender  # Add -lXrender here

# Define directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Define source and object files
SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/PathValidator.cpp $(SRC_DIR)/ConfigManager.cpp
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Define include directories
INCLUDES = -I$(INCLUDE_DIR) -I/usr/local/include -I/usr/local/include/FL/images -I/usr/local/include/jpeg -I/usr/local/include/yaml-cpp -I.

# Target: main executable
main: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Rule for compiling .cpp to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

# Clean up build artifacts
clean:
	rm -f $(OBJ) main
