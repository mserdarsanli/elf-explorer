#! /usr/bin/env python3

import sys

ninja_rules = '''
cxx = g++
cc = gcc
cppflags = -std=c++17 -Wall -Wpedantic -fPIC -O3 -I out/gen
nasm_cppflags = -DHAVE_CONFIG_H -I third_party/nasm/include -I third_party/nasm -I third_party/nasm/x86 -I third_party/nasm/asm

rule compile
    depfile = $out.d
    command = $cxx -MMD -MF $out.d $cppflags -c $in -o $out

rule link
    command = $cxx $in -o $out

rule run_python
    command = python3 $in > $out

build out/gen/enums.hpp: run_python src/gen_enums.py

rule nasm_compile
    depfile = $out.d
    command = $cc -MMD -MF $out.d $nasm_cppflags -c $in -o $out

rule create_archive
    command = ar rvs $out $in
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
    'src/input_buffer.cpp',
    'src/symbol_renamer.cpp',
]

nasm_objects = [ 'out/' + src.replace( '.c', '.o' ) for src in nasm_sources ]
objexp_objects = [ 'out/' + src.replace( '.cpp', '.o' ) for src in objexp_sources ]

def main():
    with open( 'build.ninja', 'w' ) as ninja:
        ninja.write( ninja_rules )

        for src, obj in zip( nasm_sources, nasm_objects ):
            ninja.write( f'build {obj}: nasm_compile {src}\n' )

        ninja.write( f'build out/third_party/nasm/disasm_lib.a: create_archive {" ".join( nasm_objects )}\n' )

        for src, obj in zip( objexp_sources, objexp_objects ):
            ninja.write( f'build {obj}: compile {src}\n' )

        ninja.write( f'build out/symbol_renamer: link {" ".join( objexp_objects ) } out/third_party/nasm/disasm_lib.a\n' )


if __name__ == "__main__":
    if len( sys.argv ) != 1:
        print( 'Usage: ./configure.py' )
        sys.exit( 1 )
    main()

