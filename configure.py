#! /usr/bin/env python3

import sys

ninja_rules = '''
cxx = g++
cc = gcc
cppflags = -std=c++17 -Wall -Wpedantic -fPIC -O3 -I out/gen
nasm_cppflags = -DHAVE_CONFIG_H -I third_party/nasm/include -I third_party/nasm -I third_party/nasm/x86 -I third_party/nasm/asm
emcc = emcc

rule compile
    depfile = $out.d
    command = $cxx -MMD -MF $out.d $cppflags -c $in -o $out

rule link
    command = $cxx $in -o $out

rule run_python
    command = python3 $in > $out

rule run_cp
    command = cp $in $out

build out/gen/enums.hpp: run_python src/gen_enums.py

rule nasm_compile
    depfile = $out.d
    command = $cc -MMD -MF $out.d $nasm_cppflags -c $in -o $out

rule create_archive
    command = ar rvs $out $in

rule emcc_compile
    depfile = $out.d
    command = $emcc -MMD -MF $out.d -g $cppflags -c $in -o $out

rule emcc_nasm_compile
    depfile = $out.d
    command = $emcc -MMD -MF $out.d -g $nasm_cppflags -c $in -o $out

rule emcc_link
    command = $emcc -s "EXPORTED_FUNCTIONS=['_run_example', '_run_with_buffer']"  -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -s ALLOW_MEMORY_GROWTH=1 $in -o $out

build out/web/hello.o.gif: run_cp web/hello.o.gif

build out/src/hello.o: compile src/hello.cpp

rule run_xxd
    command = xxd -i < $in > $out
build out/gen/hello.xxd: run_xxd out/src/hello.o

rule embed_hello_file
    command = sed -e '/EMBED_FILE_HERE/{r out/gen/hello.xxd' -e 'd}' $in > $out

build out/web/test.html: embed_hello_file web/test.html | out/gen/hello.xxd
'''

nasm_sources = [
    'src/wrap_nasm.c',
    'third_party/nasm/asm/error.c',
    'third_party/nasm/common/common.c',
    'third_party/nasm/disasm/disasm.c',
    'third_party/nasm/disasm/sync.c',
    'third_party/nasm/x86/regdis.c',
    'third_party/nasm/x86/insnsn.c',
    'third_party/nasm/x86/disp8.c',
    'third_party/nasm/x86/regs.c',
    'third_party/nasm/x86/iflag.c',
    'third_party/nasm/x86/insnsb.c',
    'third_party/nasm/x86/insnsd.c',
    'third_party/nasm/x86/regflags.c',
    'third_party/nasm/nasmlib/ilog2.c',
    'third_party/nasm/nasmlib/badenum.c',
    'third_party/nasm/nasmlib/malloc.c',
    'third_party/nasm/nasmlib/string.c',
]

objexp_sources = [
    'src/elf_structs.cpp',
    'src/html_output.cpp',
    'src/input_buffer.cpp',
    'src/symbol_renamer.cpp',
]

nasm_objects = [ 'out/cpp/' + src.replace( '.c', '.o' ) for src in nasm_sources ]
objexp_objects = [ 'out/cpp/' + src.replace( '.cpp', '.o' ) for src in objexp_sources ]

emcc_nasm_objects = [ 'out/emcc/' + src.replace( '.c', '.o' ) for src in nasm_sources ]
emcc_objexp_objects = [ 'out/emcc/' + src.replace( '.cpp', '.o' ) for src in objexp_sources ]

def main():
    with open( 'build.ninja', 'w' ) as ninja:
        ninja.write( ninja_rules )

        for src, obj in zip( nasm_sources, nasm_objects ):
            ninja.write( f'build {obj}: nasm_compile {src}\n' )

        for src, obj in zip( nasm_sources, emcc_nasm_objects ):
            ninja.write( f'build {obj}: emcc_nasm_compile {src}\n' )

        ninja.write( f'build out/cpp/disasm_lib.a: create_archive {" ".join( nasm_objects )}\n' )

        for src, obj in zip( objexp_sources, objexp_objects ):
            ninja.write( f'build {obj}: compile {src}\n' )

        for src, obj in zip( objexp_sources, emcc_objexp_objects ):
            ninja.write( f'build {obj}: emcc_compile {src}\n' )

        ninja.write( f'build out/symbol_renamer: link {" ".join( objexp_objects ) } out/cpp/disasm_lib.a\n' )
        ninja.write( f'build out/web/object_explorer.js: emcc_link {" ".join( emcc_nasm_objects + emcc_objexp_objects ) }\n' )


if __name__ == "__main__":
    if len( sys.argv ) != 1:
        print( 'Usage: ./configure.py' )
        sys.exit( 1 )
    main()
