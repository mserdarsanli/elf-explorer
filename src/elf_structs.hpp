#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
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

inline // TODO
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

inline // TODO
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

    StringTable( const unsigned char *data, uint64_t section_offset, uint64_t section_size )
        : m_str( (const char*)data + section_offset, section_size )
    {
    }

    std::string_view StringAtOffset( uint64_t string_offset ) const
    {
        return m_str.data() + string_offset;
    }

    std::string m_str;
};

extern StringTable shstrtab; // TODO this should be part of ctx/file object


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
    ELF_File( std::vector< unsigned char > &&contents_ );

    uint8_t U8At( uint64_t offset );
    uint16_t U16At( uint64_t offset );
    uint32_t U32At( uint64_t offset );
    uint64_t U64At( uint64_t offset );

    std::vector< unsigned char > contents;

    uint64_t section_header_offset;
    uint16_t section_header_entry_size;
    uint16_t section_header_num_entries;
    uint16_t section_names_header_index;

    std::optional< StringTable > strtab;
};
