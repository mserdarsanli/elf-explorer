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

    ELF_File file( std::move( contents ) );
    std::cout << "File looks fine.\n";


    bool in_read_sec = true;
    // Report unread parts of the file
    for ( size_t i = 0; i < file.m_read.size(); ++i )
    {
        bool read = file.m_read[ i ];

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
