#! /usr/bin/python3
#
# Copyright 2018 Mustafa Serdar Sanli
#
# This file is part of ELF Explorer.
#
# ELF Explorer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ELF Explorer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.


import collections
import html

Enum = collections.namedtuple( 'Enum', [ 'name', 'int_type', 'values' ] )
Bitfield = collections.namedtuple( 'Bitfield', [ 'name', 'int_type', 'values' ] )

Enums = [
    Enum( name = 'SectionType',
        int_type = 'uint32_t',
        values = [
            ( 'SHT_NULL',           0, 'Section header table entry unused' ),
            ( 'SHT_PROGBITS',       1, 'Program data' ),
            ( 'SHT_SYMTAB',         2, 'Symbol table' ),
            ( 'SHT_STRTAB',         3, 'String table' ),
            ( 'SHT_RELA',           4, 'Relocation entries with addends' ),
            ( 'SHT_HASH',           5, 'Symbol hash table' ),
            ( 'SHT_DYNAMIC',        6, 'Dynamic linking information' ),
            ( 'SHT_NOTE',           7, 'Notes' ),
            ( 'SHT_NOBITS',         8, 'Program space with no data (bss)' ),
            ( 'SHT_REL',            9, 'Relocation entries, no addends' ),
            ( 'SHT_SHLIB',         10, 'Reserved' ),
            ( 'SHT_DYNSYM',        11, 'Dynamic linker symbol table' ),
            ( 'SHT_INIT_ARRAY',    14, 'Array of constructors' ),
            ( 'SHT_FINI_ARRAY',    15, 'Array of destructors' ),
            ( 'SHT_PREINIT_ARRAY', 16, 'Array of pre-constructors' ),
            ( 'SHT_GROUP',         17, 'Section group' ),
            ( 'SHT_SYMTAB_SHNDX',  18, 'Extended section indeces' ),
            ( 'SHT_NUM',           19, 'Number of defined types' ),
        ],
    ),
    Enum( name = 'SymbolBinding',
        int_type = 'uint8_t',
        values = [
            ( 'STB_LOCAL',  0, 'Local symbol' ),
            ( 'STB_GLOBAL', 1, 'Global symbol' ),
            ( 'STB_WEAK',   2, 'Weak symbol' ),
        ],
    ),
    Enum( name = 'SymbolType',
        int_type = 'uint8_t',
        values = [
            ( 'STT_NOTYPE',  0, 'Symbol type is unspecified' ),
            ( 'STT_OBJECT',  1, 'Symbol is a data object' ),
            ( 'STT_FUNC',    2, 'Symbol is a code object' ),
            ( 'STT_SECTION', 3, 'Symbol associated with a section' ),
            ( 'STT_FILE',    4, 'Symbol\'s name is file name' ),
            ( 'STT_COMMON',  5, 'Symbol is a common data object' ),
            ( 'STT_TLS',     6, 'Symbol is thread-local data object' ),
            ( 'STT_NUM',     7, 'Number of defined types' ),
        ],
    ),
    Enum( name = 'SymbolVisibility',
        int_type = 'uint8_t',
        values = [
            ( 'STV_DEFAULT',   0, 'Default symbol visibility rules' ),
            ( 'STV_INTERNAL',  1, 'Processor specific hidden class' ),
            ( 'STV_HIDDEN',    2, 'Sym unavailable in other modules' ),
            ( 'STV_PROTECTED', 3, 'Not preemptible, not exported' ),
        ],
    ),
    # see /usr/include/elf.h for details..
    Enum( name = 'X64RelocationType',
        int_type = 'uint32_t',
        values = [
            ( 'R_X86_64_NONE',             0, 'No reloc' ),
            ( 'R_X86_64_64',               1, 'Direct 64 bit' ),
            ( 'R_X86_64_PC32',             2, 'PC relative 32 bit signed' ),
            ( 'R_X86_64_GOT32',            3, '32 bit GOT entry' ),
            ( 'R_X86_64_PLT32',            4, '32 bit PLT address' ),
            ( 'R_X86_64_COPY',             5, 'Copy symbol at runtime' ),
            ( 'R_X86_64_GLOB_DAT',         6, 'Create GOT entry' ),
            ( 'R_X86_64_JUMP_SLOT',        7, 'Create PLT entry' ),
            ( 'R_X86_64_RELATIVE',         8, 'Adjust by program base' ),
            ( 'R_X86_64_GOTPCREL',         9, '32 bit signed PC relative offset to GOT' ),
            ( 'R_X86_64_32',              10, 'Direct 32 bit zero extended' ),
            ( 'R_X86_64_32S',             11, 'Direct 32 bit sign extended' ),
            ( 'R_X86_64_16',              12, 'Direct 16 bit zero extended' ),
            ( 'R_X86_64_PC16',            13, '16 bit sign extended pc relative' ),
            ( 'R_X86_64_8',               14, 'Direct 8 bit sign extended' ),
            ( 'R_X86_64_PC8',             15, '8 bit sign extended pc relative' ),
            ( 'R_X86_64_DTPMOD64',        16, 'ID of module containing symbol' ),
            ( 'R_X86_64_DTPOFF64',        17, 'Offset in module\'s TLS block' ),
            ( 'R_X86_64_TPOFF64',         18, 'Offset in initial TLS block' ),
            ( 'R_X86_64_TLSGD',           19, '32 bit signed PC relative offset to two GOT entries for GD symbol' ),
            ( 'R_X86_64_TLSLD',           20, '32 bit signed PC relative offset to two GOT entries for LD symbol' ),
            ( 'R_X86_64_DTPOFF32',        21, 'Offset in TLS block' ),
            ( 'R_X86_64_GOTTPOFF',        22, '32 bit signed PC relative offset to GOT entry for IE symbol' ),
            ( 'R_X86_64_TPOFF32',         23, 'Offset in initial TLS block' ),
            ( 'R_X86_64_PC64',            24, 'PC relative 64 bit' ),
            ( 'R_X86_64_GOTOFF64',        25, '64 bit offset to GOT' ),
            ( 'R_X86_64_GOTPC32',         26, '32 bit signed pc relative offset to GOT' ),
            ( 'R_X86_64_GOT64',           27, '64-bit GOT entry offset' ),
            ( 'R_X86_64_GOTPCREL64',      28, '64-bit PC relative offset to GOT entry' ),
            ( 'R_X86_64_GOTPC64',         29, '64-bit PC relative offset to GOT' ),
            ( 'R_X86_64_GOTPLT64',        30, 'like GOT64, says PLT entry needed' ),
            ( 'R_X86_64_PLTOFF64',        31, '64-bit GOT relative offset to PLT entry' ),
            ( 'R_X86_64_SIZE32',          32, 'Size of symbol plus 32-bit addend' ),
            ( 'R_X86_64_SIZE64',          33, 'Size of symbol plus 64-bit addend' ),
            ( 'R_X86_64_GOTPC32_TLSDESC', 34, 'GOT offset for TLS descriptor.' ),
            ( 'R_X86_64_TLSDESC_CALL',    35, 'Marker for call through TLS descriptor.' ),
            ( 'R_X86_64_TLSDESC',         36, 'TLS descriptor.' ),
            ( 'R_X86_64_IRELATIVE',       37, 'Adjust indirectly by program base' ),
            ( 'R_X86_64_RELATIVE64',      38, '64-bit adjust by program base' ),
            ( 'R_X86_64_PC32_BND',        39, 'Reserved' ),
            ( 'R_X86_64_PLT32_BND',       40, 'Reserved' ),
            ( 'R_X86_64_GOTPCRELX',       41, 'Load from 32 bit signed pc relative offset to GOT entry without REX prefix, relaxable.' ),
            ( 'R_X86_64_REX_GOTPCRELX',   42, 'Load from 32 bit signed pc relative offset to GOT entry with REX prefix, relaxable.' ),
            ( 'R_X86_64_NUM',             43 ),
        ],
    )
]


Bitfields = [
    Bitfield( name = 'SectionFlags',
        int_type = 'uint64_t',
        values = [
            ( 'SHF_WRITE',            (1 <<  0), 'Writable' ),
            ( 'SHF_ALLOC',            (1 <<  1), 'Occupies memory during execution' ),
            ( 'SHF_EXECINSTR',        (1 <<  2), 'Executable' ),
            ( 'SHF_MERGE',            (1 <<  4), 'Might be merged' ),
            ( 'SHF_STRINGS',          (1 <<  5), 'Contains nul-terminated strings' ),
            ( 'SHF_INFO_LINK',        (1 <<  6), '`sh_info` contains SHT index' ),
            ( 'SHF_LINK_ORDER',       (1 <<  7), 'Preserve order after combining' ),
            ( 'SHF_OS_NONCONFORMING', (1 <<  8), 'Non-standard OS specific handling required' ),
            ( 'SHF_GROUP',            (1 <<  9), 'Section is member of a group' ),
            ( 'SHF_TLS',              (1 << 10), 'Section hold thread-local data' ),
            ( 'SHF_COMPRESSED',       (1 << 11), 'Section with compressed data' ),
        ],
    ),
    Enum( name = 'GroupHandling',
        int_type = 'uint32_t',
        values = [
            ( 'GRP_COMDAT',       1, 'Mark group as COMDAT' ),
        ],
    ),
]

def gen_enums_hpp():
    out = []
    out.append( '#include <iostream>' )
    out.append( '' )
    out.append( 'std::string escape( const std::string &s );' )
    out.append( '' )

    for e in Enums:
        out.append( f"enum class {e.name} : {e.int_type}" )
        out.append( "{" )
        for v in e.values:
            out.append( f"    {v[0]} = {v[1]}," )
        out.append( "};" )
        out.append( "" )
        out.append( "inline" )
        out.append( f"std::ostream& operator<<( std::ostream &out, {e.name} e )" )
        out.append( "{" )
        out.append( "    switch( e )" )
        out.append( "    {" )
        for v in e.values:
            out.append( f"    case {e.name}::{v[0]}:" )
            if len( v ) == 2:
                out.append( f"        out << \"{v[0]}\";" )
            else:
                out.append( f"        out << \"<span class=\\\"enum-val\\\" onclick=\\\"javascript:addPopup(event, '{v[0]}' );\\\">{v[0]}</span>\";" )
            out.append( f"        return out;" )
        out.append( "    }" )
        out.append( "    out << \"Unknown( \" << static_cast< int >( e ) << \" )\";" )
        out.append( "    return out;" )
        out.append( "}" )

    for b in Bitfields:
        out.append( f"enum class {b.name} : {b.int_type}" )
        out.append( "{" )
        for v in b.values:
            out.append( f"    {v[0]} = {v[1]}," )
        out.append( "};" )
        out.append( "" )

        out.append( "inline" )
        out.append( f"bool operator&( {b.name} a, {b.name} b )" )
        out.append( "{" )
        out.append( f"    return static_cast< {b.int_type} >( a ) & static_cast< {b.int_type} >( b );" )
        out.append( "}" )
        out.append( "" )

        out.append( "inline" )
        out.append( f"std::ostream& operator<<( std::ostream &out, {b.name} b )" )
        out.append( "{" )
        out.append( f"    bool need_separator = false;" )

        for v in b.values:
            out.append( f"    if ( b & {b.name}::{v[0]} )" )
            out.append(  "    {" )
            out.append(  "        if ( need_separator )" )
            out.append(  "        {" )
            out.append(  "            out << \" | \";" )
            out.append(  "        }" )
            out.append(  "        need_separator = true;" )
            if len( v ) == 2:
                out.append( f"        out << \"{v[0]}\";" )
            else:
                out.append( f"        out << \"<span class=\\\"enum-val\\\" onclick=\\\"javascript:addPopup(event, '{v[0]}' );\\\">{v[0]}</span>\";" )
            out.append( f"        b = static_cast< {b.name} >( static_cast< { b.int_type } >( b ) - {v[1]} );" )
            out.append(  "    }" )

        out.append( f"    if ( static_cast< {b.int_type} >( b ) != 0 )" )
        out.append( "    {" )
        out.append( "        if ( need_separator )" )
        out.append( "        {" )
        out.append( "            out << \" | \";" )
        out.append( "        }" )
        out.append( "        out << \"Unknown( \" << static_cast< int >( b ) << \" )\";" )
        out.append( "    }" )
        out.append( "    return out;" )
        out.append( "}" )
        out.append( "" )

    with open( 'out/gen/enums.hpp', 'w' ) as f:
        f.write( '\n'.join( out ) + '\n' )

def gen_enums_js():
    with open( 'out/gen/enums.js', 'w' ) as f:
        f.write( 'let enum_info = {\n' )
        for e in Enums + Bitfields:
            f.write( f"\n" )
            f.write( f"    // Enum values for {e.name}\n" )
            f.write( f"\n" )
            for v in e.values:
                if len( v ) > 2:
                    f.write( f"    {v[0]}: \"{v[2]}\",\n" )
                else:
                    f.write( f"    {v[0]}: \"NO_DATA\",\n" )
        f.write( '};\n' )

gen_enums_hpp()
gen_enums_js()
