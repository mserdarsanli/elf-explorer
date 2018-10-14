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
