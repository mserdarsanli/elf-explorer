#include "html_output.hpp"

#include <sstream>

#include <cxxabi.h>

static std::string demangle( const std::string &mangled_name )
{
    int status;
    char *demangled_ = ::__cxxabiv1::__cxa_demangle( mangled_name.c_str(), nullptr, nullptr, &status );

    if ( status != 0 )
    {
        std::cerr << "Demangling failed for name: " << mangled_name << ", status: " << status << "\n";
        return mangled_name;
    }

    std::string demangled = demangled_;
    free( demangled_ );
    return demangled;
}


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

void RenderSymbolTable( std::ostream &html_out, const std::vector< Symbol > &symbols )
{
    html_out << R"(
<table id="table-symbols" border="1" cellspacing="0" style="word-break: break-all;">
  <thead>
    <tr id="symbols-header-row">
      <th>Symbol</th>
      <th width="200">Name</th>
      <th>Bind</th>
      <th>Type</th>
      <th>Visibility</th>
      <th>Section Idx</th>
      <th>Value</th>
      <th>Size</th>
    </tr>
  </thead>
  <tbody>
)";

    for ( size_t i = 0; i < symbols.size(); ++i )
    {
        const Symbol &s = symbols[ i ];
        html_out << "<td>" << i << "</td>"
                 << "<td>" << escape( demangle( s.m_name ) ) << "</td>"
                 << "<td>" << s.m_binding << "</td>"
                 << "<td>" << s.m_type << "</td>"
                 << "<td>" << s.m_visibility << "</td>"
                 << "<td>" << s.m_section_idx << "</td>"
                 << "<td>" << s.m_value << "</td>"
                 << "<td>" << s.m_size << "</td>"
                 << "</tr>";
    }
    html_out << "</tbody></table>";
}
