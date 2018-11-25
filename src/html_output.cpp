#include "html_output.hpp"

#include <sstream>

void RenderAsStringTable( std::ostream &html_out, std::string_view s )
{
    if ( s.size() == 0 )
    {
        return;
    }

    html_out << "<table><tr><th>String Offset</th><th>Value</th></tr>";

    bool row_open = false;
    for ( size_t i = 0; i < s.size(); ++i )
    {
        if ( !row_open )
        {
            html_out << "<tr><td>" << i << "</td><td>";
            row_open = true;
        }

        if ( s[ i ] == 0 )
        {
            html_out << "</td></tr>";
            row_open = false;
        }
        else
        {
            html_out << escape( std::string( 1, s[ i ] ) ); // TODO assert that this is always a printable char
        }
    }

    html_out << "</table>";
    // TODO assert row_open == false
}

std::string escape( const std::string &s )
{
    std::string res;

    for ( char c : s )
    {
        switch ( c )
        {
        case '<': res += "&lt;"; break;
        case '>': res += "&gt;"; break;
        case '&': res += "&amp;"; break;
        case '"': res += "&quot;"; break;
        default: res += c;
        }
    }

    return res;
}

