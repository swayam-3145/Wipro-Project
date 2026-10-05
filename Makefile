CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2
SOURCES = src/main.cpp src/sha256.cpp src/scanner.cpp src/repository.cpp src/audit.cpp

fiaudit: $(SOURCES)
	$(CXX) $(CXXFLAGS) -Isrc $(SOURCES) -o $@

clean:
	rm -f fiaudit

.PHONY: clean
