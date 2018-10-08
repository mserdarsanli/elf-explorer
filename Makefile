
.PHONY: clean
clean:
	rm -rf out

out/prog1: src/prog1.cpp
	@mkdir -p out
	g++ -o $@ $<

out/prog2: src/prog2.cpp
	@mkdir -p out
	g++ -o $@ $<
