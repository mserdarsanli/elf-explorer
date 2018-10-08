#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

#define ASSERT( expr ) \
    if ( !( expr ) ) \
    { \
        throw std::runtime_error( "Assertion failed: " + std::string( #expr ) ); \
    }

uint16_t LoadU16( const unsigned char *data )
{
    uint16_t res = 0;
    res += data[ 1 ];
    res <<= 8;
    res += data[ 0 ];
    return res;
}

int main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "Usage: symbol_renamer <obj_file_name>\n";
        return 1;
    }

    std::vector< unsigned char > contents;
    // TODO no error checks done
    {
        std::ifstream input_file( argv[ 1 ], std::ios::binary | std::ios::ate );
        auto file_size = input_file.tellg();
        input_file.seekg( 0, std::ios::beg );

        contents.resize( file_size );
        input_file.read( (char*)contents.data(), file_size );
    }

    ASSERT( contents[ 0 ] == 0x7F );
    ASSERT( contents[ 1 ] == 'E' );
    ASSERT( contents[ 2 ] == 'L' );
    ASSERT( contents[ 3 ] == 'F' );

    ASSERT( contents[ 4 ] == 2 ); // 64-bit
    ASSERT( contents[ 5 ] == 1 ); // Little-Endian
    ASSERT( contents[ 6 ] == 1 ); // ELF version 1
    ASSERT( contents[ 7 ] == 0 ); // Not sure why this is 0
    ASSERT( contents[ 8 ] == 0 ); // Unused
    // PAD 9-15

    ASSERT( LoadU16( contents.data() + 16 ) == 1 ); // ET_REL (relocatable file)
    ASSERT( LoadU16( contents.data() + 18 ) == 0x3E ); // x86-64

    std::cout << "File looks fine.\n";

    return 0;
}
