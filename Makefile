CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -Iinclude
SRC      := src/main.cpp src/catalog.cpp
BIN      := arena

$(BIN): $(SRC) $(wildcard include/*.hpp)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

run: $(BIN)
	./$(BIN) | tee results/sample_output.txt

menu: $(BIN)
	./$(BIN) --menu

clean:
	rm -f $(BIN) $(BIN).exe

.PHONY: run menu clean
