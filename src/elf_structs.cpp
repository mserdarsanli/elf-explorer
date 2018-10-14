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


    // Extract section offsets first
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        uint64_t header_offset = section_header_offset + section_header_entry_size * i;
        section_offsets.push_back( U64At( header_offset + 0x18 ) );
    }
    std::sort( section_offsets.begin(), section_offsets.end() );


    // TODO make this a member var
    uint64_t shstrtab_header_offset = section_header_offset + section_header_entry_size * section_names_header_index;
    uint64_t shstrtab_offset = U64At( shstrtab_header_offset + 0x18 );
    std::cout << "Initializing shstrtab\n";
    shstrtab = StringTable( *this, shstrtab_offset );

    std::optional< SectionHeader > symtab_header;

    std::cout << "Parsing section headers:\n";
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        std::cout << "\n- SectionHeader[ " << i << " ]\n";
        SectionHeader sh( *this, section_header_offset + section_header_entry_size * i );
        sh.Dump();

        if ( sh.m_name == ".symtab" )
        {
            symtab_header = sh;
        }

        if ( sh.m_name == ".strtab" )
        {
            std::cout << "Initializing strtab\n";
            strtab = StringTable( *this, sh.m_offset );
        }
    }

    ASSERT( strtab );
    ASSERT( symtab_header );
    ASSERT( symtab_header->m_ent_size == 24 );
    uint64_t symtab_offset = symtab_header->m_offset;
    uint64_t symtab_elem_cnt = GetSectionSize( symtab_offset ) / 24;
    std::cout << "Fount symtab elem count = " << symtab_elem_cnt << "\n";
    ASSERT( symtab_elem_cnt != 0 );

    for ( uint64_t i = 0; i < symtab_elem_cnt; ++i )
    {
        Symbol s( *this, symtab_offset + 24 * i );
        s.Dump();
    }
}

uint8_t ELF_File::U8At( uint64_t offset ) const
{
    ASSERT( offset + 1 <= contents.size() );
    uint8_t res = contents[ offset ];
    return res;
}

uint16_t ELF_File::U16At( uint64_t offset ) const
{
    ASSERT( offset + 2 <= contents.size() );
    uint16_t res = 0;
    res <<= 8; res += contents[ offset + 1 ];
    res <<= 8; res += contents[ offset + 0 ];
    return res;
}

uint32_t ELF_File::U32At( uint64_t offset ) const
{
    ASSERT( offset + 4 <= contents.size() );
    uint32_t res = 0;
    res <<= 8; res += contents[ offset + 3 ];
    res <<= 8; res += contents[ offset + 2 ];
    res <<= 8; res += contents[ offset + 1 ];
    res <<= 8; res += contents[ offset + 0 ];
    return res;
}

uint64_t ELF_File::U64At( uint64_t offset ) const
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

SectionHeader::SectionHeader( const ELF_File &ctx, uint64_t offset )
{
    m_name = ctx.shstrtab->StringAtOffset( ctx.U32At( offset + 0x00 ) );
    m_type = static_cast< SectionType >( ctx.U32At( offset + 0x04 ) );
    m_attrs      = SectionFlagsBitfield( ctx.U64At( offset + 0x08 ) );
    m_address    = ctx.U64At( offset + 0x10 );
    m_offset     = ctx.U64At( offset + 0x18 );
    m_asso_idx   = ctx.U32At( offset + 0x28 );
    m_info       = ctx.U32At( offset + 0x2c );
    m_addr_align = ctx.U64At( offset + 0x30 );
    m_ent_size   = ctx.U64At( offset + 0x38 );
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
    std::cout << "Symbol\n";
    std::cout << "  - name = " << m_name << "\n";
    std::cout << "  - bind = " << to_string( m_binding ) << "\n";
    std::cout << "  - type = " << to_string( m_type ) << "\n";
    std::cout << "  - visibility = " << to_string( m_visibility ) << "\n";
    std::cout << "  - section idx = " << m_section_idx << "\n";
    std::cout << "  - value = " << m_value << "\n";
    std::cout << "  - size = " << m_size << "\n";
}

// Since sections sizes are not stored, this computes it by finding offset fidderence
// with the next section. Not sure if this is correct.
uint64_t ELF_File::GetSectionSize( uint64_t section_offset ) const
{
    auto it = std::lower_bound( section_offsets.begin(), section_offsets.end(), section_offset );
    ASSERT( it != section_offsets.end() );
    ASSERT( *it == section_offset );

    ++it;
    uint64_t end_offset = ( it != section_offsets.end() ? *it : contents.size() );
    return end_offset - section_offset;
}

StringTable::StringTable( const ELF_File &ctx, uint64_t section_offset )
{
    m_str.assign( (const char*)ctx.contents.data() + section_offset, ctx.GetSectionSize( section_offset ) );
}

std::string_view StringTable::StringAtOffset( uint64_t string_offset ) const
{
    return m_str.data() + string_offset;
}
