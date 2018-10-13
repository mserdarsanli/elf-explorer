CC = g++ -std=c++17 -Wall -Wpedantic

.PHONY: clean
clean:
	rm -rf out

out/prog1: src/prog1.cpp
	@mkdir -p out
	$(CC) -o $@ $<

out/prog1.o: src/prog1.cpp
	@mkdir -p out
	$(CC) -c -o $@ $<

out/prog2: src/prog2.cpp
	@mkdir -p out
	$(CC) -o $@ $<

out/prog2.o: src/prog2.cpp
	@mkdir -p out
	$(CC) -c -o $@ $<


out/symbol_renamer: src/symbol_renamer.cpp $(wildcard src/*.hpp)
	@mkdir -p out
	$(CC) -o $@ $<
