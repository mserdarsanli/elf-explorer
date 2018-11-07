#include "elf_structs.hpp"

static void DumpBinaryData( std::string_view s )
{
    if ( s.size() == 0 )
    {
        return;
    }

    const int indent = 4;
    std::cout << "In Binary:\n";
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
            render_print << ( isprint( c ) ? (char)c : '.' );
            render_hex << " " << hex( c / 16 ) << hex( c % 16 );
        }
        for ( ; j < 20 ; ++j )
        {
            render_print << " ";
        }

        std::cout << std::string( indent, ' ' ) << render_print.str() << "  " << render_hex.str() << "\n";
    }
}

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
    std::cout << "Section header offset = " << section_header_offset << "\n";

    ASSERT( input.U32At( 0x30 ) == 0 ); // Flags

    ASSERT( input.U16At( 0x34 ) == 64 ); // ELF Header size
    ASSERT( input.U16At( 0x36 ) == 0 ); // Size of program header
    ASSERT( input.U16At( 0x38 ) == 0 ); // program header num entries

    section_header_entry_size = input.U16At( 0x3A );
    std::cout << "Section header entry size = " << section_header_entry_size << "\n";

    section_header_num_entries = input.U16At( 0x3C );
    std::cout << "Section header num entries = " << section_header_num_entries << "\n";

    section_names_header_index = input.U16At( 0x3E );
    std::cout << "Section names header index = " << section_names_header_index << "\n";


    // Extract section offsets first
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        uint64_t header_offset = section_header_offset + section_header_entry_size * i;
        section_offsets.push_back( input.U64At( header_offset + 0x18 ) );
    }
    std::sort( section_offsets.begin(), section_offsets.end() );

    std::cout << "Section offsets:";
    for ( uint64_t o : section_offsets )
    {
        std::cout << "  " << o << ",";
    }
    std::cout << "\n";


    // TODO make this a member var
    uint64_t shstrtab_header_offset = section_header_offset + section_header_entry_size * section_names_header_index;
    uint64_t shstrtab_offset = input.U64At( shstrtab_header_offset + 0x18 );
    std::cout << "Initializing shstrtab\n";
    shstrtab = StringTable( *this, shstrtab_offset, input.U64At( shstrtab_header_offset + 0x20 ) );

    std::optional< SectionHeader > symtab_header;

    std::vector< SectionHeader > section_headers;
    section_headers.reserve( section_header_num_entries );
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        section_headers.emplace_back( *this, section_header_offset + section_header_entry_size * i );
    }

    std::cout << "Section headers:\n";
    for ( size_t i = 0; i < section_headers.size(); ++i )
    {
        const SectionHeader &sh = section_headers[ i ];
        std::cout << "\n- SectionHeader[ " << i << " ]" << std::endl;
        sh.Dump();

        uint64_t begin = sh.m_offset;
        uint64_t end = begin + sh.m_size;

        if ( sh.m_type == SectionType::Group )
        {
            DumpGroupSection( sh.m_offset, sh.m_size );
        }

        if ( sh.m_type == SectionType::ProgramData )
        {
            if ( (int)sh.m_attrs.m_val & (int)SectionFlags::Executable )
            {
                for ( auto i = begin; i < end; ++i )
                {
                    // Read by external prog, mark them here
                    (void)input.U8At( i );
                }

                std::stringstream cmd;
                cmd << "/bin/bash -c \"ndisasm -b64 <( dd if=" << input.file_name << " ibs=1 skip=" << begin << " count=" << end - begin << " 2>/dev/null )\"";
                std::cout << "Disassembly via command: " << cmd.str() << ":" << std::endl;
                system( cmd.str().c_str() );
            }
            else
            {
                DumpBinaryData( input.StringViewAt( sh.m_offset, sh.m_size ) );
            }
        }

        if ( sh.m_type == SectionType::Nobits || sh.m_type == SectionType::Constructors )
        {
            DumpBinaryData( input.StringViewAt( sh.m_offset, sh.m_size ) );
        }

        if ( sh.m_type == SectionType::RelocationEntries )
        {
            ASSERT( sh.m_ent_size == 24 );
            ASSERT( sh.m_size % 24 == 0 );

            for ( uint64_t i = 0; sh.m_offset + 24 * i < sh.m_offset + sh.m_size; ++i )
            {
                uint64_t ent_offset = sh.m_offset + 24 * i;

                uint64_t offset = input.U64At( ent_offset + 0x00 );
                uint32_t sym    = input.U32At( ent_offset + 0x08 );
                uint32_t type   = input.U32At( ent_offset + 0x0c );
                int64_t addend  = input.U64At( ent_offset + 0x10 );

                std::cout << "RelocationEntry[ " << i << " ]:\n";
                std::cout << "  - offset = " << offset << "\n";
                std::cout << "  - sym = " << sym << "\n";
                std::cout << "  - type = " << type << "\n";
                std::cout << "  - addend = " << addend << "\n";
            }
        }

        if ( sh.m_type == SectionType::StringTable )
        {
            std::cout << "Dumping StringTable:\n";
            for ( auto i = begin; i < end; ++i )
            {
                char c = (char)input.U8At( i );
                std::cout << ( isprint( c ) ? c : '.' );
            }
            std::cout << "\n";
        }

        if ( sh.m_name == ".symtab" )
        {
            symtab_header = sh;
        }

        if ( sh.m_name == ".strtab" )
        {
            std::cout << "Initializing strtab\n";
            strtab = StringTable( *this, sh.m_offset, sh.m_size );
        }

        Section &sec = m_sections.emplace_back();
        sec.m_name = sh.m_name;
        if ( sh.m_type == SectionType::StringTable )
        {
            sec.m_var.emplace< StringTableSection >();
        }
    }


    ASSERT( strtab );
    ASSERT( symtab_header );
    ASSERT( symtab_header->m_ent_size == 24 );
    uint64_t symtab_offset = symtab_header->m_offset;
    uint64_t symtab_elem_cnt = symtab_header->m_size / 24;
    std::cout << "Fount symtab elem count = " << symtab_elem_cnt << "\n";
    ASSERT( symtab_elem_cnt != 0 );

    for ( uint64_t i = 0; i < symtab_elem_cnt; ++i )
    {
        Symbol s( *this, symtab_offset + 24 * i );
        std::cout << "Symbol[ " << i << " ]\n";
        s.Dump();
    }
}

void ELF_File::DumpGroupSection( uint64_t offset, uint64_t size ) const
{
    ASSERT( size % 4 == 0 );

    ASSERT( input.U32At( offset ) == 0x01 ); // GRP_COMDAT ( no other option )

    std::cout << "    Dumping GROUP section at " << offset << " with size " << size << "\n";
    std::cout << "    - flags: GRP_COMDAT\n";

    uint64_t it = offset + 4;
    uint64_t end = offset + size;

    for ( ; it != end; it += 4 )
    {
        std::cout << "    - section_header_idx : " << input.U32At( it ) << "\n";
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

void SectionHeader::Dump() const
{
    std::cout << "  - name      = " << m_name << "\n";
    std::cout << "  - type      = " << m_type << "\n";
    if ( m_attrs.m_val )
        std::cout << "  - attrs     = " << to_string( m_attrs ) << "\n";
    if ( m_address )
        std::cout << "  - address   = " << m_address << "\n";
    if ( m_offset )
        std::cout << "  - offset    = " << m_offset << "\n";
    std::cout << "  - size  = " << m_size << "\n";
    if ( m_asso_idx )
        std::cout << "  - asso idx  = " << m_asso_idx << "\n";
    if ( m_info )
        std::cout << "  - info      = " << m_info << "\n";
    if ( m_addr_align )
        std::cout << "  - addralign = " << m_addr_align << "\n";
    if ( m_ent_size )
        std::cout << "  - entsize   = " << m_ent_size << "\n";
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

void Symbol::Dump() const
{
    std::cout << "  - name = " << m_name << "\n";
    std::cout << "  - bind = " << m_binding << "\n";
    std::cout << "  - type = " << m_type << "\n";
    std::cout << "  - visibility = " << m_visibility << "\n";
    std::cout << "  - section idx = " << m_section_idx << "\n";
    std::cout << "  - value = " << m_value << "\n";
    std::cout << "  - size = " << m_size << "\n";
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
