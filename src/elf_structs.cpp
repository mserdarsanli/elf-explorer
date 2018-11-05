#include "elf_structs.hpp"

ELF_File::ELF_File( std::string_view file_name, std::vector< unsigned char > &&contents_ )
    : contents( std::move( contents_ ) )
    , m_read( contents.size(), false )
{
    ASSERT( U8At( 0 ) == 0x7F );
    ASSERT( U8At( 1 ) == 'E' );
    ASSERT( U8At( 2 ) == 'L' );
    ASSERT( U8At( 3 ) == 'F' );

    ASSERT( U8At( 4 ) == 2 ); // 64-bit
    ASSERT( U8At( 5 ) == 1 ); // Little-Endian
    ASSERT( U8At( 6 ) == 1 ); // ELF version 1
    ASSERT( U8At( 7 ) == 0 ); // Not sure why this is 0
    ASSERT( U8At( 8 ) == 0 ); // Unused

    for ( int i = 9; i <= 15; ++i )
    {
        ASSERT( U8At( i ) == 0 ); // Force read these bytes ...
    }

    ASSERT( U16At( 0x10 ) == 1 ); // ET_REL (relocatable file)
    ASSERT( U16At( 0x12 ) == 0x3E ); // x86-64

    ASSERT( U32At( 0x14 ) == 1 ); // ELF v1

    ASSERT( U64At( 0x18 ) == 0 ); // Entry point offset
    ASSERT( U64At( 0x20 ) == 0 ); // Program header offset

    section_header_offset = U64At( 0x28 );
    std::cout << "Section header offset = " << section_header_offset << "\n";

    ASSERT( U32At( 0x30 ) == 0 ); // Flags

    ASSERT( U16At( 0x34 ) == 64 ); // ELF Header size
    ASSERT( U16At( 0x36 ) == 0 ); // Size of program header
    ASSERT( U16At( 0x38 ) == 0 ); // program header num entries

    section_header_entry_size = U16At( 0x3A );
    std::cout << "Section header entry size = " << section_header_entry_size << "\n";

    section_header_num_entries = U16At( 0x3C );
    std::cout << "Section header num entries = " << section_header_num_entries << "\n";

    section_names_header_index = U16At( 0x3E );
    std::cout << "Section names header index = " << section_names_header_index << "\n";


    // Extract section offsets first
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        uint64_t header_offset = section_header_offset + section_header_entry_size * i;
        section_offsets.push_back( U64At( header_offset + 0x18 ) );
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
    uint64_t shstrtab_offset = U64At( shstrtab_header_offset + 0x18 );
    std::cout << "Initializing shstrtab\n";
    shstrtab = StringTable( *this, shstrtab_offset, U64At( shstrtab_header_offset + 0x20 ) );

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

        if ( sh.m_type == SectionType::ProgramData || sh.m_type == SectionType::Nobits )
        {
            if ( (int)sh.m_attrs.m_val & (int)SectionFlags::Executable )
            {
                for ( auto i = begin; i < end; ++i )
                {
                    // Read by external prog, mark them here
                    (void)U8At( i );
                }

                std::stringstream cmd;
                cmd << "/bin/bash -c \"ndisasm -b64 <( dd if=" << file_name << " ibs=1 skip=" << begin << " count=" << end - begin << " 2>/dev/null )\"";
                std::cout << "Disassembly via command: " << cmd.str() << ":" << std::endl;
                system( cmd.str().c_str() );
            }
            else
            {
                std::cout << "Program Data in binary:\n";
                for ( uint64_t i = 0; i < sh.m_size; i += 20 )
                {
                    std::stringstream render_print;
                    std::stringstream render_hex;

                    uint64_t j = 0;
                    for ( ; j < 20 && j + i < sh.m_size; ++j )
                    {
                        auto hex = []( int a ) -> char
                        {
                            if ( a < 10 ) return '0' + a;
                            return a - 10 + 'a';
                        };

                        uint8_t c = U8At( begin + i + j );
                        render_print << ( isprint( c ) ? (char)c : '.' );
                        render_hex << " " << hex( c / 16 ) << hex( c % 16 );
                    }
                    for ( ; j < 20 ; ++j )
                    {
                        render_print << " ";
                    }

                    std::cout << render_print.str() << "  " << render_hex.str() << "\n";
                }
            }
        }

        if ( sh.m_type == SectionType::StringTable )
        {
            std::cout << "Dumping StringTable:\n";
            for ( auto i = begin; i < end; ++i )
            {
                char c = (char)U8At( i );
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

    ASSERT( U32At( offset ) == 0x01 ); // GRP_COMDAT ( no other option )

    std::cout << "    Dumping GROUP section at " << offset << " with size " << size << "\n";
    std::cout << "    - flags: GRP_COMDAT\n";

    uint64_t it = offset + 4;
    uint64_t end = offset + size;

    for ( ; it != end; it += 4 )
    {
        std::cout << "    - section_header_idx : " << U32At( it ) << "\n";
    }
}

void ELF_File::SetRead( uint64_t offset ) const
{
    m_read[ offset ] = true;
}

uint8_t ELF_File::U8At( uint64_t offset ) const
{
    ASSERT( offset + 1 <= contents.size() );
    SetRead( offset );
    uint8_t res = contents[ offset ];
    return res;
}

uint16_t ELF_File::U16At( uint64_t offset ) const
{
    ASSERT( offset + 2 <= contents.size() );
    uint16_t res = 0;
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

uint32_t ELF_File::U32At( uint64_t offset ) const
{
    ASSERT( offset + 4 <= contents.size() );
    uint32_t res = 0;
    res <<= 8; res += U8At( offset + 3 );
    res <<= 8; res += U8At( offset + 2 );
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

uint64_t ELF_File::U64At( uint64_t offset ) const
{
    ASSERT( offset + 8 <= contents.size() );
    uint64_t res = 0;
    res <<= 8; res += U8At( offset + 7 );
    res <<= 8; res += U8At( offset + 6 );
    res <<= 8; res += U8At( offset + 5 );
    res <<= 8; res += U8At( offset + 4 );
    res <<= 8; res += U8At( offset + 3 );
    res <<= 8; res += U8At( offset + 2 );
    res <<= 8; res += U8At( offset + 1 );
    res <<= 8; res += U8At( offset + 0 );
    return res;
}

SectionHeader::SectionHeader( const ELF_File &ctx, uint64_t offset )
{
    m_name = ctx.shstrtab->StringAtOffset( ctx.U32At( offset + 0x00 ) );
    m_type = static_cast< SectionType >( ctx.U32At( offset + 0x04 ) );
    m_attrs      = SectionFlagsBitfield( ctx.U64At( offset + 0x08 ) );
    m_address    = ctx.U64At( offset + 0x10 );
    m_offset     = ctx.U64At( offset + 0x18 );
    m_size       = ctx.U64At( offset + 0x20 );
    m_asso_idx   = ctx.U32At( offset + 0x28 );
    m_info       = ctx.U32At( offset + 0x2c );
    m_addr_align = ctx.U64At( offset + 0x30 );
    m_ent_size   = ctx.U64At( offset + 0x38 );
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
    m_name = ctx.strtab->StringAtOffset( ctx.U32At( offset ) );
    uint8_t info = ctx.U8At( offset + 4 );
    m_binding = static_cast< SymbolBinding >( info >> 4 );
    m_type = static_cast< SymbolType >( info & 15 );
    m_visibility = static_cast< SymbolVisibility >( ctx.U8At( offset + 5 ) );
    m_section_idx = ctx.U16At( offset + 6 );
    m_value = ctx.U64At( offset + 8 );
    m_size = ctx.U64At( offset + 16 );
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
        ctx.SetRead( section_offset + i );
    }
    m_str.assign( (const char*)ctx.contents.data() + section_offset, size );
}

std::string_view StringTable::StringAtOffset( uint64_t string_offset ) const
{
    return m_str.data() + string_offset;
}
