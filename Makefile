
.PHONY: clean
clean:
	rm -rf out

out/prog1: src/prog1.cpp
	@mkdir -p out
	g++ -o $@ $<

out/prog1.o: src/prog1.cpp
	@mkdir -p out
	g++ -c -o $@ $<

out/prog2: src/prog2.cpp
	@mkdir -p out
	g++ -o $@ $<

out/prog2.o: src/prog2.cpp
	@mkdir -p out
	g++ -c -o $@ $<


out/symbol_renamer: src/symbol_renamer.cpp
	@mkdir -p out
	g++ -o $@ $<
