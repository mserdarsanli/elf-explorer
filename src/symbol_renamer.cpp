#include <algorithm>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "elf_structs.hpp"

static std::vector< unsigned char > read_file( const char *file_name )
{
    std::vector< unsigned char > contents;

    std::ifstream input_file;
    input_file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    input_file.open( file_name, std::ios::binary | std::ios::ate );
    auto file_size = input_file.tellg();
    input_file.seekg( 0, std::ios::beg );

    contents.resize( file_size );
    input_file.read( (char*)contents.data(), file_size );

    return contents;
}


int main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "Usage: symbol_renamer <obj_file_name>\n";
        return 1;
    }

    InputBuffer input( argv[ 1 ], read_file( argv[ 1 ] ) );
    ELF_File file( input );
    std::cout << "File looks fine.\n";


    bool in_read_sec = true;
    // Report unread parts of the file
    for ( size_t i = 0; i < input.m_read.size(); ++i )
    {
        bool read = input.m_read[ i ];

        if ( in_read_sec )
        {
            if ( ! read )
            {
                std::cout << "Unread [ " << i << ", ";
                in_read_sec = false;
            }
        }
        else
        {
            if ( read )
            {
                std::cout << i << " )\n";
                in_read_sec = true;
            }
        }
    }

    return 0;
}
