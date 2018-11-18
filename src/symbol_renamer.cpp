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

    std::stringstream html_out;
    file.render_html_into( html_out );
    html_out << "File looks fine.\n";


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
<script>
  console.log( 'Enabling float Thead for: ', $('#table-section-headers') );
  $('#table-section-headers').floatThead({
position: 'fixed'
});
</script>
</body></html>
)";

    std::cout << html_out.str();

    return 0;
}
