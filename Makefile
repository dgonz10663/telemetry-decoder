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

.PHONY: all clean run

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

# Remove build artifacts
clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

# Quick test command
run: $(TARGET)
	./$(TARGET) --help



# Build & run CRC unit tests
TESTBIN := bin/test_crc8
TESTSRC := tests/test_crc8.cpp

.PHONY: test
test: $(TESTBIN)
	./$(TESTBIN)

$(TESTBIN): $(TESTSRC) src/CRC8.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $(TESTSRC) src/CRC8.cpp

