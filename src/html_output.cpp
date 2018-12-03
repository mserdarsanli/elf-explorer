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

void RenderSectionHeaders( std::ostream &html_out,
                           const std::vector< SectionHeader > &section_headers )
{
    html_out << R"(
<table id="table-section-headers" border="1" cellspacing="0" style="word-break: break-all;">
  <thead>
    <tr id="section-headers-header-row">
      <th>Section Header</th>
      <th width="200">Name</th>
      <th>Type</th>
      <th>Attrs</th>
      <th>Address</th>
      <th>Offset</th>
      <th>Size</th>
      <th>Asso Idx</th>
      <th>Info</th>
      <th>Addr Align</th>
      <th>Ent Size</th>
    </tr>
  </thead>
  <tbody>
)";

    for ( size_t i = 1; i != section_headers.size(); ++i )
    {
        const SectionHeader &sh = section_headers[ i ];

        html_out << "<tr>"
                 << "<td><a class=\"section_header_anchor\" name=\"section-header-" << i << "\"></a><a href=\"#section-header-" << i << "\">" << i << "</a></td>"
                 << "<td>" << escape( sh.m_name ) << "</td>"
                 << "<td>" << sh.m_type << "</td>"
                 << "<td>" << escape( to_string( sh.m_attrs ) ) << "</td>"
                 << "<td>" << sh.m_address << "</td>"
                 << "<td><a href=\"#section-" << i << "\">" << sh.m_offset << "</a></td>"
                 << "<td>" << sh.m_size << "</td>"
                 << "<td>" << sh.m_asso_idx << "</td>"
                 << "<td>" << sh.m_info << "</td>"
                 << "<td>" << sh.m_addr_align << "</td>"
                 << "<td>" << sh.m_ent_size << "</td>"
                 << "</tr>";
    }
    html_out << "</tbody></table>";
}

void RenderSectionTitle( std::ostream &html_out, size_t i, const SectionHeader &sh )
{
    html_out << R"(<div style="background-color: #eeeeee;">)";
    html_out << R"(<table style="text-align: left;" border="0" cellspacing="0">)";
    html_out << "<tr><th colspan=\"2\"><a style=\"font-size: 200%;\" name=\"section-" << i << "\">Section " << i << ": " << escape( sh.m_name ) << "</a></th></tr>";
    html_out << "<tr><th>Name</th><td>" << escape( sh.m_name ) << "</td></tr>";
    html_out << "<tr><th>Type</th><td>" << sh.m_type << "</td></tr>";
    html_out << "<tr><th>Attrs</th><td>" << escape( to_string( sh.m_attrs ) ) << "</td></tr>";
    html_out << "<tr><th>Address</th><td>" << sh.m_address << "</td></tr>";
    html_out << "<tr><th>Size</th><td>" << sh.m_size << "</td></tr>";
    html_out << "<tr><th>Asso Idx</th><td>" << sh.m_asso_idx << "</td></tr>";
    html_out << "<tr><th>Info</th><td>" << sh.m_info << "</td></tr>";
    html_out << "<tr><th>Addr Align</th><td>" << sh.m_addr_align << "</td></tr>";
    html_out << "<tr><th>Ent Size</th><td>" << sh.m_ent_size << "</td></tr>";
    html_out << R"(</table>)";
    html_out << R"(</div>)";
}
