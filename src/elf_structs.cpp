#include "elf_structs.hpp"
#include "html_output.hpp"
#include "wrap_nasm.h"

ELF_File::ELF_File( InputBuffer &input_ )
    : input( input_ )
{
    ASSERT( input.U8At( 0 ) == 0x7F );
    ASSERT( input.U8At( 1 ) == 'E' );
    ASSERT( input.U8At( 2 ) == 'L' );
    ASSERT( input.U8At( 3 ) == 'F' );

    ASSERT( input.U8At( 4 ) == 2 ); // 64-bit
    ASSERT( input.U8At( 5 ) == 1 ); // Little-Endian
    ASSERT( input.U8At( 6 ) == 1 ); // ELF version 1
    ASSERT( input.U8At( 7 ) == 0 || input.U8At( 7 ) == 3 ); // Not sure why this is 0
    ASSERT( input.U8At( 8 ) == 0 ); // Unused

    for ( int i = 9; i <= 15; ++i )
    {
        ASSERT( input.U8At( i ) == 0 ); // Force read these bytes ...
    }

    ASSERT( input.U16At( 0x10 ) == 1 ); // ET_REL (relocatable file)
    ASSERT( input.U16At( 0x12 ) == 0x3E ); // x86-64

    ASSERT( input.U32At( 0x14 ) == 1 ); // ELF v1

    ASSERT( input.U64At( 0x18 ) == 0 ); // Entry point offset
    ASSERT( input.U64At( 0x20 ) == 0 ); // Program header offset

    section_header_offset = input.U64At( 0x28 );
    ASSERT( input.U32At( 0x30 ) == 0 ); // Flags

    ASSERT( input.U16At( 0x34 ) == 64 ); // ELF Header size
    ASSERT( input.U16At( 0x36 ) == 0 ); // Size of program header
    ASSERT( input.U16At( 0x38 ) == 0 ); // program header num entries

    section_header_entry_size = input.U16At( 0x3A );
    section_header_num_entries = input.U16At( 0x3C );
    section_names_header_index = input.U16At( 0x3E );


    // TODO make this a member var
    uint64_t shstrtab_header_offset = section_header_offset + section_header_entry_size * section_names_header_index;
    uint64_t shstrtab_offset = input.U64At( shstrtab_header_offset + 0x18 );
    shstrtab = StringTable( *this, shstrtab_offset, input.U64At( shstrtab_header_offset + 0x20 ) );

    m_section_headers.reserve( section_header_num_entries );
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        m_section_headers.emplace_back( *this, section_header_offset + section_header_entry_size * i );
    }

    for ( size_t i = 0; i < m_section_headers.size(); ++i )
    {
        const SectionHeader &sh = m_section_headers[ i ];

        // TODO remove this?
        if ( sh.m_name == ".strtab" )
        {
            strtab = StringTable( *this, sh.m_offset, sh.m_size );
        }
    }

    // Load actual section data
    // TODO recursively load dependent sections first
    m_sections.resize( m_section_headers.size() );
    for ( size_t i = 1; i < m_section_headers.size(); ++i )
    {
        const SectionHeader &sh = m_section_headers[ i ];

        switch ( sh.m_type )
        {
        case SectionType::SHT_STRTAB:
        {
            m_sections[ i ].m_var = StringTable( *this, sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_SYMTAB:
        {
            ASSERT( sh.m_ent_size == 24 );

            SymbolTable &symtab = m_sections[ i ].m_var.emplace< SymbolTable >();

            symtab.m_symbols.reserve( sh.m_size / sh.m_ent_size );

            for ( uint64_t i = 0; i * 24 < sh.m_size; ++i )
            {
                symtab.m_symbols.emplace_back( *this, sh.m_offset + 24 * i );
            }

            break;
        }
        case SectionType::SHT_RELA:
        {
            ASSERT( sh.m_ent_size == 24 );
            ASSERT( sh.m_size % 24 == 0 );

            RelocationEntries &entries = m_sections[ i ].m_var.emplace< RelocationEntries >();
            entries.m_entries.resize( sh.m_size / 24 );

            for ( uint64_t i = 0; sh.m_offset + 24 * i < sh.m_offset + sh.m_size; ++i )
            {
                uint64_t ent_offset = sh.m_offset + 24 * i;

                entries.m_entries[ i ].m_offset = input.U64At( ent_offset + 0x00 );
                entries.m_entries[ i ].m_type   = static_cast< X64RelocationType >( input.U32At( ent_offset + 0x08 ) );
                entries.m_entries[ i ].m_symbol = input.U32At( ent_offset + 0x0c );
                entries.m_entries[ i ].m_addend = input.U64At( ent_offset + 0x10 );
            }
            break;
        }
        case SectionType::SHT_GROUP:
        {
            ASSERT( sh.m_size % 4 == 0 );
            GroupSection &group = m_sections[ i ].m_var.emplace< GroupSection >();

            group.m_flags = this->input.U32At( sh.m_offset );

            uint64_t it = sh.m_offset + 4;
            uint64_t end = sh.m_offset + sh.m_size;

            for ( ; it != end; it += 4 )
            {
                group.m_section_indices.push_back( this->input.U32At( it ) );
            }
            break;
        }
        case SectionType::SHT_NOBITS:
        {
            auto &s = m_sections[ i ].m_var.emplace< NoBitsSection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_INIT_ARRAY:
        {
            auto &s = m_sections[ i ].m_var.emplace< InitArraySection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_PROGBITS:
        {
            auto &s = m_sections[ i ].m_var.emplace< ProgBitsSection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            s.m_is_executable = ( (int)sh.m_attrs.m_val & (int)SectionFlags::Executable );
            break;
        }
        default:
            std::cerr << "Skipping unhandled section of type " << sh.m_type << "\n";
        }
    }


    ASSERT( strtab );
}

struct SectionHtmlRenderer
{
    SectionHtmlRenderer( std::ostream &html_out_ )
        : html_out( html_out_ )
    {
    }

    void operator()( const std::monostate & )
    {
        // Do nothing?
    }

    void operator()( const NoBitsSection &s )
    {
        RenderBinaryData( html_out, s.m_data );
    }

    void operator()( const ProgBitsSection &s )
    {
        if ( s.m_is_executable )
        {
            std::stringstream disasm_out;

            auto fp = []( const char *ins, void *data )
            {
                *static_cast< std::stringstream* >( data ) << ins;
            };

            html_out << "Disassembly:<br>";

            DisasmExecutableSection( (const unsigned char *)s.m_data.data(), s.m_data.size(), fp, static_cast< void* >( &disasm_out ) );

            html_out << "<pre style=\"padding-left: 100px;\">" << escape( disasm_out.str() ) << "</pre>";
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
        RenderSymbolTable( html_out, symtab.m_symbols );
    }

    void operator()( const GroupSection &group )
    {
        ASSERT( group.m_flags == 0x01 ); // GRP_COMDAT ( no other option )

        html_out << "GROUP section<br>"
                 << "    - flags: GRP_COMDAT<br>";

        for ( uint32_t sec_idx : group.m_section_indices )
        {
            html_out << "    - section idx : " << sec_idx << "<br>";
        }
    }

    void operator()( const RelocationEntries &reloc )
    {
        html_out << "<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\"><tr><th>Relocation Entry</th><th>Offset</th><th>Sym</th><th>Type</th><th>Addend</th></tr>";
        for ( size_t entry_idx = 0; entry_idx < reloc.m_entries.size(); ++entry_idx )
        {
            html_out << "<tr>"
                     << "<td>" << entry_idx << "</td>"
                     << "<td>" << reloc.m_entries[ entry_idx ].m_offset << "</td>"
                     << "<td>" << reloc.m_entries[ entry_idx ].m_symbol << "</td>"
                     << "<td>" << reloc.m_entries[ entry_idx ].m_type << "</td>"
                     << "<td>" << reloc.m_entries[ entry_idx ].m_addend << "</td>"
                     << "</tr>";
        }
        html_out << "</table>";
    }

    std::ostream &html_out;
};

void ELF_File::render_html_into( std::ostream &html_out )
{
    html_out << R"(<!doctype html>
<html>
  <head>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/floatthead/2.1.2/jquery.floatThead.min.js"></script>
    <link rel="stylesheet" type="text/css" href="style.css">
  </head>
  <body>
)";

    html_out << "<h2>Section Headers</h2>";
    RenderSectionHeaders( html_out, m_section_headers );

    for ( size_t i = 1; i < m_section_headers.size(); ++i )
    {
        const SectionHeader &sh = m_section_headers[ i ];

        RenderSectionTitle( html_out, i, sh );

        std::visit( SectionHtmlRenderer( html_out ), m_sections[ i ].m_var );

        std::cerr << "<script>console.log( 'unknown section' );</script>\n";
    }


}

SectionHeader::SectionHeader( const ELF_File &ctx, uint64_t offset )
{
    m_name = ctx.shstrtab->StringAtOffset( ctx.input.U32At( offset + 0x00 ) );
    m_type = static_cast< SectionType >( ctx.input.U32At( offset + 0x04 ) );
    m_attrs      = SectionFlagsBitfield( ctx.input.U64At( offset + 0x08 ) );
    m_address    = ctx.input.U64At( offset + 0x10 );
    m_offset     = ctx.input.U64At( offset + 0x18 );
    m_size       = ctx.input.U64At( offset + 0x20 );
    m_asso_idx   = ctx.input.U32At( offset + 0x28 );
    m_info       = ctx.input.U32At( offset + 0x2c );
    m_addr_align = ctx.input.U64At( offset + 0x30 );
    m_ent_size   = ctx.input.U64At( offset + 0x38 );
}

Symbol::Symbol( const ELF_File &ctx, uint64_t offset )
{
    m_name = ctx.strtab->StringAtOffset( ctx.input.U32At( offset ) );
    uint8_t info = ctx.input.U8At( offset + 4 );
    m_binding = static_cast< SymbolBinding >( info >> 4 );
    m_type = static_cast< SymbolType >( info & 15 );
    m_visibility = static_cast< SymbolVisibility >( ctx.input.U8At( offset + 5 ) );
    m_section_idx = ctx.input.U16At( offset + 6 );
    m_value = ctx.input.U64At( offset + 8 );
    m_size = ctx.input.U64At( offset + 16 );
}

StringTable::StringTable( const ELF_File &ctx, uint64_t section_offset, uint64_t size )
{
    for ( uint64_t i = 0; i < size; ++i )
    {
        (void)ctx.input.U8At( section_offset + i ); // Set read
    }
    m_str.assign( (const char*)ctx.input.contents.data() + section_offset, size );
}

std::string_view StringTable::StringAtOffset( uint64_t string_offset ) const
{
    return m_str.data() + string_offset;
}
