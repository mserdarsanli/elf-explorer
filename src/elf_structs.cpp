#include "elf_structs.hpp"

ELF_File ELF_File::LoadFrom( InputBuffer &input )
{
    ELF_File res;

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

    uint64_t section_header_offset = input.U64At( 0x28 );
    ASSERT( input.U32At( 0x30 ) == 0 ); // Flags

    ASSERT( input.U16At( 0x34 ) == 64 ); // ELF Header size
    ASSERT( input.U16At( 0x36 ) == 0 ); // Size of program header
    ASSERT( input.U16At( 0x38 ) == 0 ); // program header num entries

    uint16_t section_header_entry_size = input.U16At( 0x3A );
    uint16_t section_header_num_entries = input.U16At( 0x3C );
    uint16_t section_names_header_index = input.U16At( 0x3E );


    // TODO make this a member var
    uint64_t shstrtab_header_offset = section_header_offset + section_header_entry_size * section_names_header_index;
    uint64_t shstrtab_offset = input.U64At( shstrtab_header_offset + 0x18 );
    res.shstrtab = StringTable( input, shstrtab_offset, input.U64At( shstrtab_header_offset + 0x20 ) );

    res.m_section_headers.reserve( section_header_num_entries );
    for ( int i = 0; i < section_header_num_entries; ++i )
    {
        res.m_section_headers.emplace_back( input, *res.shstrtab, section_header_offset + section_header_entry_size * i );
    }

    for ( size_t i = 0; i < res.m_section_headers.size(); ++i )
    {
        const SectionHeader &sh = res.m_section_headers[ i ];

        // TODO remove this?
        if ( sh.m_name == ".strtab" )
        {
            res.strtab = StringTable( input, sh.m_offset, sh.m_size );
        }
    }

    // Load actual section data
    // TODO recursively load dependent sections first
    res.m_sections.resize( res.m_section_headers.size() );
    for ( size_t i = 1; i < res.m_section_headers.size(); ++i )
    {
        const SectionHeader &sh = res.m_section_headers[ i ];

        switch ( sh.m_type )
        {
        case SectionType::SHT_STRTAB:
        {
            res.m_sections[ i ].m_var = StringTable( input, sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_SYMTAB:
        {
            ASSERT( sh.m_ent_size == 24 );

            SymbolTable &symtab = res.m_sections[ i ].m_var.emplace< SymbolTable >();

            symtab.m_symbols.reserve( sh.m_size / sh.m_ent_size );

            for ( uint64_t i = 0; i * 24 < sh.m_size; ++i )
            {
                symtab.m_symbols.emplace_back( input, *res.strtab, sh.m_offset + 24 * i );
            }

            break;
        }
        case SectionType::SHT_RELA:
        {
            ASSERT( sh.m_ent_size == 24 );
            ASSERT( sh.m_size % 24 == 0 );

            RelocationEntries &entries = res.m_sections[ i ].m_var.emplace< RelocationEntries >();
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
            GroupSection &group = res.m_sections[ i ].m_var.emplace< GroupSection >();

            group.m_flags = input.U32At( sh.m_offset );

            uint64_t it = sh.m_offset + 4;
            uint64_t end = sh.m_offset + sh.m_size;

            for ( ; it != end; it += 4 )
            {
                group.m_section_indices.push_back( input.U32At( it ) );
            }
            break;
        }
        case SectionType::SHT_NOBITS:
        {
            auto &s = res.m_sections[ i ].m_var.emplace< NoBitsSection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_INIT_ARRAY:
        {
            auto &s = res.m_sections[ i ].m_var.emplace< InitArraySection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            break;
        }
        case SectionType::SHT_PROGBITS:
        {
            auto &s = res.m_sections[ i ].m_var.emplace< ProgBitsSection >();
            s.m_data = input.StringViewAt( sh.m_offset, sh.m_size );
            s.m_is_executable = ( (int)sh.m_attrs.m_val & (int)SectionFlags::Executable );
            break;
        }
        default:
            std::cerr << "Skipping unhandled section of type " << sh.m_type << "\n";
        }
    }

    ASSERT( res.strtab );

    return res;
}

SectionHeader::SectionHeader( InputBuffer &input, StringTable &shstrtab, uint64_t offset )
{
    m_name = shstrtab.StringAtOffset( input.U32At( offset + 0x00 ) );
    m_type = static_cast< SectionType >( input.U32At( offset + 0x04 ) );
    m_attrs      = SectionFlagsBitfield( input.U64At( offset + 0x08 ) );
    m_address    = input.U64At( offset + 0x10 );
    m_offset     = input.U64At( offset + 0x18 );
    m_size       = input.U64At( offset + 0x20 );
    m_asso_idx   = input.U32At( offset + 0x28 );
    m_info       = input.U32At( offset + 0x2c );
    m_addr_align = input.U64At( offset + 0x30 );
    m_ent_size   = input.U64At( offset + 0x38 );
}

Symbol::Symbol( InputBuffer &input, StringTable &strtab, uint64_t offset )
{
    m_name = strtab.StringAtOffset( input.U32At( offset ) );
    uint8_t info = input.U8At( offset + 4 );
    m_binding = static_cast< SymbolBinding >( info >> 4 );
    m_type = static_cast< SymbolType >( info & 15 );
    m_visibility = static_cast< SymbolVisibility >( input.U8At( offset + 5 ) );
    m_section_idx = input.U16At( offset + 6 );
    m_value = input.U64At( offset + 8 );
    m_size = input.U64At( offset + 16 );
}

StringTable::StringTable( InputBuffer &input, uint64_t section_offset, uint64_t size )
{
    for ( uint64_t i = 0; i < size; ++i )
    {
        (void)input.U8At( section_offset + i ); // Set read
    }
    m_str.assign( (const char*)input.contents.data() + section_offset, size );
}

std::string_view StringTable::StringAtOffset( uint64_t string_offset ) const
{
    return m_str.data() + string_offset;
}
