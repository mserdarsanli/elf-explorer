#include "html_output.hpp"

#include <sstream>

void RenderAsStringTable( std::ostream &html_out, std::string_view s )
{
    if ( s.size() == 0 )
    {
        return;
    }

    html_out << "<pre style=\"padding-left: 100px;\">";
    for ( char c : s )
    {
        html_out << escape( std::string( 1, isprint( c ) ? c : '.' ) );
        if ( c == '\0' )
        {
            html_out << "\n";
        }
    }
    html_out << "</pre>";
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

