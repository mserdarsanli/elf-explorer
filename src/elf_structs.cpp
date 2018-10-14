#include "elf_structs.hpp"

ELF_File::ELF_File( std::vector< unsigned char > &&contents_ )
    : contents( std::move( contents_ ) )
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
    // PAD 9-15

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


    std::vector< uint64_t > section_offsets;
    {
        for ( int i = 0; i < section_header_num_entries; ++i )
        {
            uint64_t header_offset = section_header_offset + section_header_entry_size * i;
            section_offsets.push_back( U64At( header_offset + 0x18 ) );
        }
        std::sort( section_offsets.begin(), section_offsets.end() );
    }


    // TODO make this a member var
    uint64_t shstrtab_header_offset = section_header_offset + section_header_entry_size * section_names_header_index;
    uint64_t shstrtab_offset = U64At( shstrtab_header_offset + 0x18 );
    shstrtab = StringTable( contents.data(), shstrtab_offset, contents.size() - shstrtab_offset );

    std::optional< SectionHeader > symtab_header;

    std::cout << "Parsing section headers:\n";
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        std::cout << "\n- SectionHeader[ " << i << " ]\n";
        SectionHeader sh( *this, contents.data(), section_header_offset + section_header_entry_size * i );
        sh.Dump();

        if ( sh.m_name == ".symtab" )
        {
            symtab_header = sh;
        }

        if ( sh.m_name == ".strtab" )
        {
            strtab = StringTable( contents.data(), sh.m_offset, contents.size() - sh.m_offset );
        }
    }

    ASSERT( strtab );
    ASSERT( symtab_header );
    ASSERT( symtab_header->m_ent_size == 24 );
    uint64_t symtab_offset = symtab_header->m_offset;
    uint64_t symtab_elem_cnt = 0;

    // Find symtab size
    {
        auto it = std::upper_bound( section_offsets.begin(), section_offsets.end(), symtab_offset );
        ASSERT( it != section_offsets.end() );
        uint64_t next_offset = *it;

        ASSERT( ( next_offset - symtab_offset ) % 24 == 0 );
        symtab_elem_cnt = ( next_offset - symtab_offset ) / 24;
    }
    std::cout << "Fount symtab elem count = " << symtab_elem_cnt << "\n";
    ASSERT( symtab_elem_cnt != 0 );

    for ( uint64_t i = 0; i < symtab_elem_cnt; ++i )
    {
        Symbol s( contents.data(), *strtab, symtab_offset + 24 * i );
        s.Dump();
    }
}

uint8_t ELF_File::U8At( uint64_t offset )
{
    ASSERT( offset + 1 <= contents.size() );
    uint8_t res = contents[ offset ];
    return res;
}

uint16_t ELF_File::U16At( uint64_t offset )
{
    ASSERT( offset + 2 <= contents.size() );
    uint16_t res = 0;
    res <<= 8; res += contents[ offset + 1 ];
    res <<= 8; res += contents[ offset + 0 ];
    return res;
}

uint32_t ELF_File::U32At( uint64_t offset )
{
    ASSERT( offset + 4 <= contents.size() );
    uint32_t res = 0;
    res <<= 8; res += contents[ offset + 3 ];
    res <<= 8; res += contents[ offset + 2 ];
    res <<= 8; res += contents[ offset + 1 ];
    res <<= 8; res += contents[ offset + 0 ];
    return res;
}

uint64_t ELF_File::U64At( uint64_t offset )
{
    ASSERT( offset + 8 <= contents.size() );
    uint64_t res = 0;
    res <<= 8; res += contents[ offset + 7 ];
    res <<= 8; res += contents[ offset + 6 ];
    res <<= 8; res += contents[ offset + 5 ];
    res <<= 8; res += contents[ offset + 4 ];
    res <<= 8; res += contents[ offset + 3 ];
    res <<= 8; res += contents[ offset + 2 ];
    res <<= 8; res += contents[ offset + 1 ];
    res <<= 8; res += contents[ offset + 0 ];
    return res;
}

SectionHeader::SectionHeader( const ELF_File &ctx, const unsigned char *data, uint64_t header_offset )
{
    const unsigned char *sh = data + header_offset;

    m_name = ctx.shstrtab->StringAtOffset( LoadU32( sh + 0x00 ) );
    m_type = static_cast< SectionType >( LoadU32( sh + 0x04 ) );
    m_attrs      = SectionFlagsBitfield( LoadU64( sh + 0x08 ) );
    m_address    = LoadU64( sh + 0x10 );
    m_offset     = LoadU64( sh + 0x18 );
    m_asso_idx   = LoadU32( sh + 0x28 );
    m_info       = LoadU32( sh + 0x2c );
    m_addr_align = LoadU64( sh + 0x30 );
    m_ent_size   = LoadU64( sh + 0x38 );
}

void SectionHeader::Dump() const
{
    std::cout << "  - name      = " << m_name << "\n";
    std::cout << "  - type      = " << to_string( m_type ) << " (" << (int)m_type << ")\n";
    if ( m_attrs.m_val )
        std::cout << "  - attrs     = " << to_string( m_attrs ) << "\n";
    if ( m_address )
        std::cout << "  - address   = " << m_address << "\n";
    if ( m_offset )
        std::cout << "  - offset    = " << m_offset << "\n";
    if ( m_asso_idx )
        std::cout << "  - asso idx  = " << m_asso_idx << "\n";
    if ( m_info )
        std::cout << "  - info      = " << m_info << "\n";
    if ( m_addr_align )
        std::cout << "  - addralign = " << m_addr_align << "\n";
    if ( m_ent_size )
        std::cout << "  - entsize   = " << m_ent_size << "\n";
}
