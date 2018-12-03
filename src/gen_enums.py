#! /usr/bin/python3

import collections

Enum = collections.namedtuple( 'Enum', [ 'name', 'int_type', 'values' ] )

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
            out.append( f"        out << \"<abbr title=\\\"{v[2]}\\\">{v[0]}</abbr>\";" )
        out.append( f"        return out;" )
    out.append( "    }" )
    out.append( "    out << \"\\033[31mUnknown( \" << static_cast< int >( e ) << \" )\\033[0m\";" )
    out.append( "    return out;" )
    out.append( "}" )

print( '\n'.join( out ) )
