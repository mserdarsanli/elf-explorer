// Copyright 2018-2019 Mustafa Serdar Sanli
//
// This file is part of ELF Explorer.
//
// ELF Explorer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ELF Explorer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.


#include "html_output.hpp"

// TODO usa fmtlib and remove ios & iomanip
#include <ios>
#include <iomanip>

#include <sstream>

#include <cxxabi.h>

#include <fmt/format.h>

#include "wrap_nasm.h"

namespace elfexplorer {

struct Anchor
{
    static
    std::string ForSection( size_t idx )
    {
        return fmt::format( "section-{}", idx );
    }

    static
    std::string ForSectionHeader( size_t idx )
    {
        return fmt::format( "section-header-{}", idx );
    }

    static
    std::string ForSymbol( size_t section_idx, size_t symbol_idx )
    {
        return fmt::format( "section-{}-symbol-{}", section_idx, symbol_idx );
    }
};

struct Link
{
    static
    std::string ToSection( const std::vector< Section > &sections, size_t idx )
    {
        if ( idx <= 0 || idx > sections.size() )
        {
            return fmt::format( "{}", idx );
        }
        return fmt::format( R"(<a href="#{}">Section {} ({})</a>)", Anchor::ForSection( idx ), idx, escape( sections[ idx ].m_header.m_name ) );
    }

    static
    std::string ToSymbol( const std::vector< Section > &sections, size_t section_idx, size_t symbol_idx )
    {
        if ( section_idx == 0 || section_idx > sections.size() )
        {
            return fmt::format( "Symbol {}", symbol_idx );
        }
        const Section &sec = sections.at( section_idx );

        if ( ! std::holds_alternative< SymbolTable >( sec.m_var ) )
        {
            return fmt::format( "Symbol {}", symbol_idx );
        }

        const SymbolTable &symtab = std::get< SymbolTable >( sec.m_var );

        if ( symbol_idx == 0 || symbol_idx > symtab.m_symbols.size() )
        {
            return fmt::format( "Symbol {}", symbol_idx );
        }

        const std::string &sym_name =  symtab.m_symbols[ symbol_idx ].m_name;
        if ( sym_name.size() )
        {
            return fmt::format( R"(<a href="#{}">Symbol {} ({})</a>)", Anchor::ForSymbol( section_idx, symbol_idx ), symbol_idx, escape( sym_name ) );
        }
        else
        {
            return fmt::format( R"(<a href="#{}">Symbol {}</a>)", Anchor::ForSymbol( section_idx, symbol_idx ), symbol_idx );
        }
    }
};

static void RenderAsStringTable( std::ostream &html_out, std::string_view s )
{
    if ( s.size() == 0 )
    {
        return;
    }

    html_out << "<table class=\"sticky-header\"><tr><th>String Offset</th><th>Value</th></tr>";

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

static void RenderSectionHeaders( std::ostream &html_out,
                           const std::vector< Section > &sections )
{
    html_out << R"(
<table class="sticky-header" border="1" cellspacing="0" cellpadding="3" style="word-break: break-all;">
  <thead>
    <tr>
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

    for ( size_t i = 1; i != sections.size(); ++i )
    {
        const SectionHeader &sh = sections[ i ].m_header;

        html_out << "<tr>"
                 << "<td><a class=\"sticky-anchor\" name=\"" << Anchor::ForSectionHeader( i ) << "\"></a><a href=\"#" << Anchor::ForSectionHeader( i ) << "\">" << i << "</a></td>"
                 << "<td>" << escape( sh.m_name ) << "</td>"
                 << "<td>" << sh.m_type << "</td>"
                 << "<td>" << sh.m_attrs << "</td>"
                 << "<td>" << sh.m_address << "</td>"
                 << "<td><a href=\"#" << Anchor::ForSection( i ) << "\">" << sh.m_offset << "</a></td>"
                 << "<td>" << sh.m_size << "</td>"
                 << "<td>" << Link::ToSection( sections, sh.m_asso_idx ) << "</td>";

        if ( sh.m_type == SectionType::SHT_GROUP )
        {
            html_out << "<td>" << Link::ToSymbol( sections, sh.m_asso_idx, sh.m_info ) << "</td>";
        }
        else
        {
            html_out << "<td>" << sh.m_info << "</td>";
        }

        html_out << "<td>" << sh.m_addr_align << "</td>"
                 << "<td>" << sh.m_ent_size << "</td>"
                 << "</tr>";
    }
    html_out << "</tbody></table>";
}

static void RenderSectionTitle( std::ostream &html_out, const std::vector< Section > &sections, size_t i )
{
    const SectionHeader &sh = sections[ i ].m_header;

    html_out << R"(<div class="section-title">)";
    html_out << R"(<table style="text-align: left;" border="0" cellspacing="0">)";
    html_out << "<tr><th colspan=\"2\"><a style=\"font-size: 200%;\" name=\"" << Anchor::ForSection( i ) << "\">Section " << i << ": " << escape( sh.m_name ) << "</a></th></tr>";
    html_out << "<tr><th>Name</th><td>" << escape( sh.m_name ) << "</td></tr>";
    html_out << "<tr><th>Type</th><td>" << sh.m_type << "</td></tr>";
    html_out << "<tr><th>Attrs</th><td>" << sh.m_attrs << "</td></tr>";
    html_out << "<tr><th>Address</th><td>" << sh.m_address << "</td></tr>";
    html_out << "<tr><th>Size</th><td>" << sh.m_size << "</td></tr>";
    html_out << "<tr><th>Asso Idx</th><td>" << Link::ToSection( sections, sh.m_asso_idx ) << "</td></tr>";
    html_out << "<tr><th>Info</th><td>" << sh.m_info << "</td></tr>";
    html_out << "<tr><th>Addr Align</th><td>" << sh.m_addr_align << "</td></tr>";
    html_out << "<tr><th>Ent Size</th><td>" << sh.m_ent_size << "</td></tr>";
    html_out << R"(</table>)";
    html_out << R"(</div>)";
}

static void RenderBinaryData( std::ostream &html_out, std::string_view s )
{
    if ( s.size() == 0 )
    {
        return;
    }

    const int indent = 4;
    html_out << "<pre style=\"padding-left: 100px;\">";
    for ( uint64_t i = 0; i < s.size(); i += 20 )
    {
        std::stringstream render_print;
        std::stringstream render_hex;

        uint64_t j = 0;
        for ( ; j < 20 && j + i < s.size(); ++j )
        {
            auto hex = []( int a ) -> char
            {
                if ( a < 10 ) return '0' + a;
                return a - 10 + 'a';
            };

            uint8_t c = s[ i + j ];
            if ( isprint( c ) )
            {
                render_print << escape( std::string( 1, c ) );
            }
            else
            {
                render_print << '.';
            }
            render_hex << " " << hex( c / 16 ) << hex( c % 16 );
        }
        for ( ; j < 20 ; ++j )
        {
            render_print << " ";
        }

        html_out << std::string( indent, ' ' ) << render_print.str() << "  " << render_hex.str() << "\n";
    }
    html_out << "</pre>";
}

struct SectionHtmlRenderer
{
    SectionHtmlRenderer( std::ostream &html_out_, const std::vector< Section > &sections, size_t sec_idx )
        : html_out( html_out_ )
        , m_sections( sections )
        , m_cur_section_idx( sec_idx )
    {
    }

    void operator()( const std::monostate & )
    {
        std::cerr << "<script>console.log( 'unknown section' );</script>\n";
    }

    void operator()( const NoBitsSection &s )
    {
        RenderBinaryData( html_out, s.m_data );
    }

    void operator()( const ProgBitsSection &s )
    {
        if ( s.m_is_executable )
        {
            struct State
            {
                std::vector< RelocationEntry > reloc_entries;
                std::vector< RelocationEntry >::const_iterator reloc_it;
                int reloc_size = 4; // TODO this should be derived by reloc type
                const SymbolTable *symtab = nullptr;

                std::string_view data;
                std::stringstream disasm_out;
            };
            State state;
            state.data = s.m_data;

            // Check next section for relocation entries
            // TODO this is wrong! it could be in another section
            // TODO also check if there could be multiple relocation sections for a progbits section
            if ( m_sections[ m_cur_section_idx + 1 ].m_header.m_type == SectionType::SHT_RELA
              && m_sections[ m_cur_section_idx + 1 ].m_header.m_info == m_cur_section_idx )
            {
                state.reloc_entries = std::get< RelocationEntries >( m_sections[ m_cur_section_idx + 1 ].m_var ).m_entries;

                uint32_t symtab_idx = m_sections[ m_cur_section_idx + 1 ].m_header.m_asso_idx;
                if ( symtab_idx < m_sections.size() )
                {
                    if ( std::holds_alternative< SymbolTable >( m_sections[ symtab_idx ].m_var ) )
                    {
                        state.symtab = &std::get< SymbolTable >( m_sections[ symtab_idx ].m_var );
                    }
                }
            }
            if ( state.reloc_entries.size() )
            {
                ASSERT( state.symtab != nullptr );
            }

            std::sort( state.reloc_entries.begin(), state.reloc_entries.end(), []( const auto &a, const auto &b ){ return a.m_offset < b.m_offset; } );
            state.reloc_it = state.reloc_entries.cbegin();

            auto fp = []( int offset, int len, char *instruction_str, void *user_data )
            {
                State &st = *reinterpret_cast< State* >( user_data );

                st.disasm_out << "<tr><td>" << std::setw( 8 ) << std::setfill( '0' ) << offset << "</td><td>";
                for ( int i = 0; i < len; ++i )
                {
                    // TODO assert reloc size <= instruction size
                    if ( st.reloc_it != st.reloc_entries.cend() && st.reloc_it->m_offset == size_t( offset + i ) )
                    {
                        st.disasm_out << R"(<span style="color:red; cursor: pointer;">)";
                    }
                    st.disasm_out << std::hex << std::setw( 2 ) << (int)static_cast< unsigned char >( st.data[ offset + i ] ) << " " << std::dec;
                    if ( st.reloc_it != st.reloc_entries.cend() && st.reloc_it->m_offset + st.reloc_size - 1 == size_t( offset + i ) )
                    {
                        const RelocationEntry &e = *st.reloc_it;
                        st.disasm_out << "&lt;" << e.m_type << " , " << st.symtab->m_symbols[ e.m_symbol ].m_name << " , " << e.m_addend  << "&gt;";
                        st.disasm_out << R"(</span>)";
                        ++st.reloc_it;
                    }
                }

                st.disasm_out << "</td><td>" << escape( instruction_str ) << "</td></tr>";
            };

            DisasmExecutableSection( reinterpret_cast< unsigned char* >( const_cast< char* >( s.m_data.data() ) ), s.m_data.size(), fp, static_cast< void* >( &state ) );

            html_out << "<div class=\"assembly-code\"><table>" << state.disasm_out.str() << "</table></div>";
        }
        else
        {
            RenderBinaryData( html_out, s.m_data );
        }
    }

    void operator()( const InitArraySection &s )
    {
        RenderBinaryData( html_out, s.m_data );
    }

    void operator()( const StringTable &strtab )
    {
        RenderAsStringTable( html_out, strtab.m_str );
    }

    void operator()( const SymbolTable &symtab )
    {
        const std::vector< Symbol > &symbols = symtab.m_symbols;

        html_out << R"(
    <table class="sticky-header" border="1" cellspacing="0" style="word-break: break-all;">
      <thead>
        <tr>
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
            html_out << "<td>"
                           "<a class=\"sticky-anchor\" name=\"" << Anchor::ForSymbol( m_cur_section_idx, i ) << "\"></a>"
                           "<a href=\"#" << Anchor::ForSymbol( m_cur_section_idx, i ) << "\">" << i << "</a>"
                        "</td>"
                     << "<td>" << escape( s.m_name ) << "</td>"
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

    void operator()( const GroupSection &group )
    {
        ASSERT( group.m_flags == GroupHandling::GRP_COMDAT ); // ( no other option known )

        html_out << "<table border=\"1\" cellpadding=\"3\" cellspacing=\"0\"><tr><th>Flags</th><td>" << group.m_flags << "</td></tr>";

        for ( size_t i = 0; i < group.m_section_indices.size(); ++i )
        {
            uint32_t sec_idx = group.m_section_indices[ i ];
            html_out << "<tr>";
            if ( i == 0 )
            {
                html_out << "<th rowspan=\"" << group.m_section_indices.size() << "\">Sections</th>";
            }
            html_out << "<td>" << Link::ToSection( m_sections, sec_idx ) << "</td>";
        }

        html_out << "</table>";
    }

    void operator()( const RelocationEntries &reloc )
    {
        const SectionHeader &sh = m_sections[ m_cur_section_idx ].m_header;
        const SymbolTable &symtab = std::get< SymbolTable >( m_sections[ sh.m_asso_idx ].m_var ); // TODO assert

        html_out << "<table class=\"sticky-header\" border=\"1\" cellspacing=\"0\" cellpadding=\"3\"><tr><th>Relocation Entry</th><th>Offset</th><th>Sym</th><th>Type</th><th>Addend</th></tr>";
        for ( size_t entry_idx = 0; entry_idx < reloc.m_entries.size(); ++entry_idx )
        {
            const RelocationEntry &entry = reloc.m_entries[ entry_idx ];

            html_out << "<tr>"
                     << "<td>" << entry_idx << "</td>"
                     << "<td>" << entry.m_offset << "</td>"
                     // TODO create info popup for symbols (demangled value etc.)
                     // and show that on click
                     << "<td>" << escape( symtab.m_symbols[ entry.m_symbol ].m_name ) << "</td>"
                     << "<td>" << entry.m_type << "</td>"
                     << "<td>" << entry.m_addend << "</td>"
                     << "</tr>";
        }
        html_out << "</table>";
    }

    std::ostream &html_out;
    const std::vector< Section > m_sections;
    size_t m_cur_section_idx;
};

void RenderAsHTML( std::ostream &html_out, const ELF_File &elf )
{
    html_out << R"(<!doctype html>
<html>
  <head>
    <link rel="stylesheet" type="text/css" href="style.css">
  </head>
  <body>
)";

    html_out << "<h2>Section Headers</h2>";
    RenderSectionHeaders( html_out, elf.m_sections );

    for ( size_t i = 1; i < elf.m_sections.size(); ++i )
    {
        RenderSectionTitle( html_out, elf.m_sections, i );

        std::visit( SectionHtmlRenderer( html_out, elf.m_sections, i ), elf.m_sections[ i ].m_var );
    }

    html_out << R"(
</body></html>
)";
}

} // namespace elfexplorer
