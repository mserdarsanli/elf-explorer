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
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
    return res;
}

uint32_t LoadU32( const unsigned char *data )
{
    uint32_t res = 0;
    res <<= 8; res += data[ 3 ];
    res <<= 8; res += data[ 2 ];
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
    return res;
}

uint64_t LoadU64( const unsigned char *data )
{
    uint64_t res = 0;
    res <<= 8; res += data[ 7 ];
    res <<= 8; res += data[ 6 ];
    res <<= 8; res += data[ 5 ];
    res <<= 8; res += data[ 4 ];
    res <<= 8; res += data[ 3 ];
    res <<= 8; res += data[ 2 ];
    res <<= 8; res += data[ 1 ];
    res <<= 8; res += data[ 0 ];
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

    ASSERT( LoadU16( contents.data() + 0x10 ) == 1 ); // ET_REL (relocatable file)
    ASSERT( LoadU16( contents.data() + 0x12 ) == 0x3E ); // x86-64

    ASSERT( LoadU32( contents.data() + 0x14 ) == 1 ); // ELF v1

    ASSERT( LoadU64( contents.data() + 0x18 ) == 0 ); // Entry point offset
    ASSERT( LoadU64( contents.data() + 0x20 ) == 0 ); // Program header offset

    uint64_t section_header_offset = LoadU64( contents.data() + 0x28 );
    std::cout << "Section header offset = " << section_header_offset << "\n";

    ASSERT( LoadU32( contents.data() + 0x30 ) == 0 ); // Flags

    ASSERT( LoadU16( contents.data() + 0x34 ) == 64 ); // ELF Header size
    ASSERT( LoadU16( contents.data() + 0x36 ) == 0 ); // Size of program header
    ASSERT( LoadU16( contents.data() + 0x38 ) == 0 ); // program header num entries

    uint16_t section_header_entry_size = LoadU16( contents.data() + 0x3A );
    std::cout << "Section header entry size = " << section_header_entry_size << "\n";

    uint16_t section_header_num_entries = LoadU16( contents.data() + 0x3C );
    std::cout << "Section header num entries = " << section_header_num_entries << "\n";

    uint16_t section_names_header_index = LoadU16( contents.data() + 0x3E );
    std::cout << "Section names header index = " << section_names_header_index << "\n";

    std::cout << "File looks fine.\n";

    return 0;
}
