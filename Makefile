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

src/enums.hpp: src/gen_enums.py
	$< > $@

out/symbol_renamer: $(wildcard src/*.cpp) $(wildcard src/*.hpp) src/enums.hpp
	@mkdir -p out
	$(CC) -o $@ src/symbol_renamer.cpp src/elf_structs.cpp
