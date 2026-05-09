Compiler = clang
CFLAGS = -fsanitize=address -Wall -Wextra -g
LIBS = -lSDL3 -lm
TARGET = GeoMati
SRC = main.c
EXTRA = libs/tinyexpr.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(Compiler) $(CFLAGS) $(SRC) $(EXTRA) -o $(TARGET) $(LIBS)
	@echo "Compilado correctamente"

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)


.PHONY: all clean run
