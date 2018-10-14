#include <algorithm>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "util.hpp"

enum class SectionType : uint32_t
{
    ProgramData = 1,
    SymbolTable = 2,
    StringTable = 3,
    RelocationEntries = 4,
    Nobits = 8,
    Constructors = 14,
    Group = 17,
};

enum class SectionFlags : uint64_t
{
    Writable   = 1 << 0,
    Alloc      = 1 << 1,
    Executable = 1 << 2,

    Merge      = 1 << 4,
    Strings    = 1 << 5,
    InfoLink   = 1 << 6,

    Group      = 1 << 9,
};

struct SectionFlagsBitfield
{
    SectionFlagsBitfield() = default;
    SectionFlagsBitfield( uint64_t val )
        : m_val( val )
    {
    }

    uint64_t m_val = 0;
};

std::string_view to_string( SectionType t )
{
    switch ( t )
    {
    case SectionType::ProgramData: return "ProgramData";
    case SectionType::SymbolTable: return "SymbolTable";
    case SectionType::StringTable: return "StringTable";
    case SectionType::RelocationEntries: return "RelocationEntries";
    case SectionType::Constructors: return "Constructors";
    case SectionType::Group: return "Group";
    case SectionType::Nobits: return "Nobits";
    }
    return "\033[31mUNKNOWN\033[0m";
}

std::string to_string( SectionFlagsBitfield f )
{
    uint64_t val = f.m_val;

    std::stringstream out;

    #define SR_PROC_BIT( name ) \
        if ( val & static_cast< uint64_t >( SectionFlags::name ) ) \
        { \
            out << #name; \
            val -= static_cast< uint64_t >( SectionFlags::name ); \
            if ( val ) \
            { \
                out << " | "; \
            } \
        }

    SR_PROC_BIT( Writable   );
    SR_PROC_BIT( Alloc      );
    SR_PROC_BIT( Executable );

    SR_PROC_BIT( Merge      );
    SR_PROC_BIT( Strings    );
    SR_PROC_BIT( InfoLink   );

    SR_PROC_BIT( Group      );

    #undef SR_PROC_BIT

    if ( val )
    {
        out << "\033[31mUnknown( " << val << " )\033[0m";
    }

    return out.str();
}

struct StringTable
{
    StringTable() = default;

    StringTable( const unsigned char *data, uint64_t header_offset )
        : m_data( data )
    {
        const unsigned char *header = m_data + header_offset;
        uint32_t s_type = LoadU32( header + 0x04 );
        ASSERT( s_type == 3 ); // SHT_STRTAB
        ASSERT( LoadU64( header + 0x30 ) == 1 ); // Addralign

        std::cout << "Created correct str table\n";
        uint64_t table_offset = LoadU64( header + 0x18 );

        std::cout << "Table offset is: " << table_offset << "\n";
        m_table = m_data + table_offset;
    }

    std::string_view StringAtOffset( uint64_t string_offset ) const
    {
        return reinterpret_cast< const char* >( m_table + string_offset );
    }

    const unsigned char *m_data;
    const unsigned char *m_table;
};

struct Symbol
{
    Symbol( const unsigned char *data, const StringTable &strtab, uint64_t offset )
    {
        const unsigned char *sym = data + offset;

        m_name = strtab.StringAtOffset( LoadU32( sym ) );
        m_info = LoadU8( sym + 4 );
        m_visibility = LoadU8( sym + 5 );
        m_section_idx = LoadU16( sym + 6 );
        m_value = LoadU64( sym + 8 );
        m_size = LoadU64( sym + 16 );
    }

    void Dump() const
    {
        std::cout << "Symbol\n";
        std::cout << "  - name = " << m_name << "\n";
        {
            std::cout << "  - info\n";
            uint8_t bind = m_info >> 4;
            std::cout << "    - bind = " << ( bind == 0 ? "Local" : bind == 1 ? "Global" : bind == 2 ? "Weak" : "Unknown" ) << "( " << (int)bind << " )\n";
            uint8_t type = m_info & 15;
            std::cout << "    - type = " << (int)type << "\n";
        }
        std::cout << "  - visibility = " << (int)m_visibility << "\n";
        std::cout << "  - section idx = " << m_section_idx << "\n";
        std::cout << "  - value = " << m_value << "\n";
        std::cout << "  - size = " << m_size << "\n";
    }

    std::string m_name;
    uint8_t m_info;
    uint8_t m_visibility;
    uint16_t m_section_idx;
    uint64_t m_value;
    uint64_t m_size;
};

StringTable shstrtab; // TODO this should be part of ctx/file object

struct SectionHeader
{
    SectionHeader( const unsigned char *data, uint64_t header_offset )
    {
        const unsigned char *sh = data + header_offset;

        m_name = shstrtab.StringAtOffset( LoadU32( sh + 0x00 ) );
        m_type = static_cast< SectionType >( LoadU32( sh + 0x04 ) );
        m_attrs      = SectionFlagsBitfield( LoadU64( sh + 0x08 ) );
        m_address    = LoadU64( sh + 0x10 );
        m_offset     = LoadU64( sh + 0x18 );
        m_asso_idx   = LoadU32( sh + 0x28 );
        m_info       = LoadU32( sh + 0x2c );
        m_addr_align = LoadU64( sh + 0x30 );
        m_ent_size   = LoadU64( sh + 0x38 );
    }

    void Dump() const
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

    std::string m_name;
    SectionType m_type;
    SectionFlagsBitfield m_attrs;
    uint64_t m_address;
    uint64_t m_offset;
    uint32_t m_asso_idx;
    uint32_t m_info;
    uint64_t m_addr_align;
    uint64_t m_ent_size;
};

struct ELF_File
{
    ELF_File( std::vector< unsigned char > &&contents_ )
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

        // TODO make this a member var
        shstrtab = StringTable( contents.data(), section_header_offset + section_header_entry_size * section_names_header_index );

        std::optional< SectionHeader > symtab_header;
        std::vector< uint64_t > section_offsets;

        std::cout << "Parsing section headers:\n";
        for ( int i = 0; i < section_header_num_entries; ++i )
        {
            std::cout << "\n- SectionHeader[ " << i << " ]\n";
            SectionHeader sh( contents.data(), section_header_offset + section_header_entry_size * i );
            sh.Dump();

            section_offsets.push_back( sh.m_offset );

            if ( sh.m_name == ".symtab" )
            {
                symtab_header = sh;
            }

            if ( sh.m_name == ".strtab" )
            {
                strtab = StringTable( contents.data(), section_header_offset + section_header_entry_size * i ); // TODO this should work with actual offset not header offset!!
            }
        }
        std::sort( section_offsets.begin(), section_offsets.end() );

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

    uint8_t U8At( uint64_t offset )
    {
        ASSERT( offset + 1 <= contents.size() );
        uint8_t res = contents[ offset ];
        return res;
    }

    uint16_t U16At( uint64_t offset )
    {
        ASSERT( offset + 2 <= contents.size() );
        uint16_t res = 0;
        res <<= 8; res += contents[ offset + 1 ];
        res <<= 8; res += contents[ offset + 0 ];
        return res;
    }

    uint32_t U32At( uint64_t offset )
    {
        ASSERT( offset + 4 <= contents.size() );
        uint32_t res = 0;
        res <<= 8; res += contents[ offset + 3 ];
        res <<= 8; res += contents[ offset + 2 ];
        res <<= 8; res += contents[ offset + 1 ];
        res <<= 8; res += contents[ offset + 0 ];
        return res;
    }

    uint64_t U64At( uint64_t offset )
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

    std::vector< unsigned char > contents;

    uint64_t section_header_offset;
    uint16_t section_header_entry_size;
    uint16_t section_header_num_entries;
    uint16_t section_names_header_index;

    std::optional< StringTable > strtab;
};

int main( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
        std::cerr << "Usage: symbol_renamer <obj_file_name>\n";
        return 1;
    }

    std::vector< unsigned char > contents;
    // TODO no error checks done
    {
        std::ifstream input_file( argv[ 1 ], std::ios::binary | std::ios::ate );
        auto file_size = input_file.tellg();
        input_file.seekg( 0, std::ios::beg );

        contents.resize( file_size );
        input_file.read( (char*)contents.data(), file_size );
    }

    ELF_File file( std::move( contents ) );
    std::cout << "File looks fine.\n";

    return 0;
}
