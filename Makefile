# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude

# Folder layout
SRCDIR   := src
BINDIR   := bin
TARGET   := $(BINDIR)/telemetry-decoder

# Gather .cpp files and map to .o files
SRC      := $(wildcard $(SRCDIR)/*.cpp)
OBJ      := $(patsubst $(SRCDIR)/%.cpp, $(SRCDIR)/%.o, $(SRC))

.PHONY: all clean run test

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# Compile each .cpp into .o
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create bin directory if missing
$(BINDIR):
	mkdir -p $(BINDIR)

# Quick test command
run: $(TARGET)
	./$(TARGET) --help


test:
	$(CXX) $(CXXFLAGS) -Iinclude -o bin/tests tests/test_main.cpp \
	src/CRC8.cpp src/TelemetryParser.cpp src/CSVLogger.cpp
	./bin/tests

clean:
	rm -f $(SRCDIR)/*.o $(TARGET) $(BINDIR)/tests

