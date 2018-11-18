FLAGS="-DHAVE_CONFIG_H -I third_party/nasm/include -I third_party/nasm -I third_party/nasm/x86 -I third_party/nasm/asm"

emcc -g -o out/f01.o $FLAGS -c src/wrap_nasm.c
emcc -g -o out/f02.o $FLAGS -c third_party/nasm/asm/error.c
emcc -g -o out/f03.o $FLAGS -c third_party/nasm/common/common.c
emcc -g -o out/f04.o $FLAGS -c third_party/nasm/disasm/disasm.c
emcc -g -o out/f05.o $FLAGS -c third_party/nasm/disasm/sync.c
emcc -g -o out/f06.o $FLAGS -c third_party/nasm/x86/regdis.c
emcc -g -o out/f07.o $FLAGS -c third_party/nasm/x86/insnsn.c
emcc -g -o out/f08.o $FLAGS -c third_party/nasm/x86/disp8.c
emcc -g -o out/f09.o $FLAGS -c third_party/nasm/x86/regs.c
emcc -g -o out/f10.o $FLAGS -c third_party/nasm/x86/iflag.c
emcc -g -o out/f11.o $FLAGS -c third_party/nasm/x86/insnsb.c
emcc -g -o out/f12.o $FLAGS -c third_party/nasm/x86/insnsd.c
emcc -g -o out/f13.o $FLAGS -c third_party/nasm/x86/regflags.c
emcc -g -o out/f14.o $FLAGS -c third_party/nasm/nasmlib/ilog2.c
emcc -g -o out/f15.o $FLAGS -c third_party/nasm/nasmlib/badenum.c
emcc -g -o out/f16.o $FLAGS -c third_party/nasm/nasmlib/malloc.c
emcc -g -o out/f17.o $FLAGS -c third_party/nasm/nasmlib/string.c

emcc -s ALLOW_MEMORY_GROWTH=1  -g -std=c++17 -I out/gen src/symbol_renamer.cpp src/elf_structs.cpp src/input_buffer.cpp out/f*.o --embed-file out/src/prog1.o
