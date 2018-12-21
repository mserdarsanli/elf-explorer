#include <algorithm>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "elf_structs.hpp"
#include "html_output.hpp"

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

std::vector< unsigned char > mem_data;
std::string mem_result;

int my_main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "Usage: symbol_renamer <obj_file_name>\n";
        return 1;
    }

    std::vector< unsigned char > obj_file_contents;
    if ( argv[ 1 ] == std::string_view( "--mem-data" ) )
    {
        obj_file_contents = mem_data;
    }
    else
    {
        obj_file_contents = read_file( argv[ 1 ] );
    }

    InputBuffer input( argv[ 1 ], std::move( obj_file_contents ) ); // TODO first parameter can be removed
    ELF_File file = ELF_File::LoadFrom( input );

    std::stringstream html_out;
    RenderAsHTML( html_out, file );

    auto begin = input.m_read.begin();
    auto end = input.m_read.end();
    auto it = begin;

    while ( true )
    {
        auto unread_begin = std::find( it, end, false );
        if ( unread_begin == end )
        {
            break;
        }

        auto unread_end = std::find( unread_begin, end, true );
        ASSERT( unread_end != end );
        it = unread_end + 1;

        auto size = unread_end - unread_begin;
        if ( size < 32 && std::all_of( unread_begin, unread_end, []( auto x ) { return x == 0; } ) )
        {
            // Probably padding, TODO also verify `unread_end` is a section start and size < sec[-1].addr_align
            continue;
        }

        html_out << "Unread [ " << unread_begin - begin << ", " << unread_end - begin << " )\n";
    }

    html_out << R"(
</body></html>
)";

    // TODO clean up this creap
    if ( argv[ 1 ] == std::string_view( "--mem-data" ) )
    {
        mem_result = html_out.str();
    }
    else
    {
        std::cout << html_out.str();
    }

    return 0;
}

int main( int argc, char* argv[] )
{
    return my_main( argc, argv );
}

extern "C" {

char* run_with_buffer( const char *data, uint64_t size )
{
    mem_data.assign( reinterpret_cast< const unsigned char * >( data ),
                     reinterpret_cast< const unsigned char * >( data ) + size );
    char arg1[] = "foo";
    char arg2[] = "--mem-data";
    char* args[3] = { arg1, arg2, nullptr };
    my_main( 2, args );

    return strdup( mem_result.c_str() );
}

} // extern "C"
