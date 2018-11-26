#! /usr/bin/python3

import collections

Enum = collections.namedtuple( 'Enum', [ 'name', 'int_type', 'values' ] )

Enums = [
    Enum( name = 'SectionType',
        int_type = 'uint32_t',
        values = [
            ( 'ProgramData', 1 ),
            ( 'SymbolTable', 2 ),
            ( 'StringTable', 3 ),
            ( 'RelocationEntries', 4 ),
            ( 'Nobits', 8 ),
            ( 'Constructors', 14 ),
            ( 'Group', 17 ),
        ],
    ),
    Enum( name = 'SymbolBinding',
        int_type = 'uint8_t',
        values = [
            ( 'Local', 0 ),
            ( 'Global', 1 ),
            ( 'Weak', 2 ),
        ],
    ),
    Enum( name = 'SymbolType',
        int_type = 'uint8_t',
        values = [
            ( 'NotSpecified', 0 ),
            ( 'Object', 1 ),
            ( 'Function', 2 ),
            ( 'Section', 3 ),
            ( 'File', 4 ),
        ],
    ),
    Enum( name = 'SymbolVisibility',
        int_type = 'uint8_t',
        values = [
            ( 'Default', 0 ),
            ( 'Internal', 1 ),
            ( 'Hidden', 2 ),
            ( 'Protected', 3 ),
        ],
    ),
    # see /usr/include/elf.h for details..
    Enum( name = 'X64RelocationType',
        int_type = 'uint32_t',
        values = [
            ( 'R_X86_64_NONE', 0 ),
            ( 'R_X86_64_64', 1 ),
            ( 'R_X86_64_PC32', 2 ),
            ( 'R_X86_64_GOT32', 3 ),
            ( 'R_X86_64_PLT32', 4 ),
            ( 'R_X86_64_COPY', 5 ),
            ( 'R_X86_64_GLOB_DAT', 6 ),
            ( 'R_X86_64_JUMP_SLOT', 7 ),
            ( 'R_X86_64_RELATIVE', 8 ),
            ( 'R_X86_64_GOTPCREL', 9 ),
            ( 'R_X86_64_32', 10 ),
            ( 'R_X86_64_32S', 11 ),
            ( 'R_X86_64_16', 12 ),
            ( 'R_X86_64_PC16', 13 ),
            ( 'R_X86_64_8', 14 ),
            ( 'R_X86_64_PC8', 15 ),
            ( 'R_X86_64_DTPMOD64', 16 ),
            ( 'R_X86_64_DTPOFF64', 17 ),
            ( 'R_X86_64_TPOFF64', 18 ),
            ( 'R_X86_64_TLSGD', 19 ),
            ( 'R_X86_64_TLSLD', 20 ),
            ( 'R_X86_64_DTPOFF32', 21 ),
            ( 'R_X86_64_GOTTPOFF', 22 ),
            ( 'R_X86_64_TPOFF32', 23 ),
            ( 'R_X86_64_PC64', 24 ),
            ( 'R_X86_64_GOTOFF64', 25 ),
            ( 'R_X86_64_GOTPC32', 26 ),
            ( 'R_X86_64_GOT64', 27 ),
            ( 'R_X86_64_GOTPCREL64', 28 ),
            ( 'R_X86_64_GOTPC64', 29 ),
            ( 'R_X86_64_GOTPLT64', 30 ),
            ( 'R_X86_64_PLTOFF64', 31 ),
            ( 'R_X86_64_SIZE32', 32 ),
            ( 'R_X86_64_SIZE64', 33 ),
            ( 'R_X86_64_GOTPC32_TLSDESC', 34 ),
            ( 'R_X86_64_TLSDESC_CALL', 35 ),
            ( 'R_X86_64_TLSDESC', 36 ),
            ( 'R_X86_64_IRELATIVE', 37 ),
            ( 'R_X86_64_RELATIVE64', 38 ),
            ( 'R_X86_64_GOTPCRELX', 41 ),
            ( 'R_X86_64_REX_GOTPCRELX', 42 ),
            ( 'R_X86_64_NUM', 43 ),
        ],
    )
]

out = []
out.append( '#include <iostream>' )
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
        out.append( f"        out << \"{v[0]}\";" )
        out.append( f"        return out;" )
    out.append( "    }" )
    out.append( "    out << \"\\033[31mUnknown( \" << static_cast< int >( e ) << \" )\\033[0m\";" )
    out.append( "    return out;" )
    out.append( "}" )

print( '\n'.join( out ) )
