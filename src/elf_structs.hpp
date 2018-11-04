#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>

#include "enums.hpp"

#define ASSERT( expr ) \
    if ( !( expr ) ) \
    { \
        throw std::runtime_error( "Assertion failed: " + std::string( #expr ) ); \
    }


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


struct ELF_File;


struct StringTable
{
    StringTable() = default;

    StringTable( const ELF_File &ctx, uint64_t section_offset, uint64_t size );
    std::string_view StringAtOffset( uint64_t string_offset ) const;

    std::string m_str;
};

struct Symbol
{
    Symbol( const ELF_File &ctx, uint64_t offset );
    void Dump() const;

    std::string m_name;
    SymbolBinding m_binding;
    SymbolType m_type;
    SymbolVisibility m_visibility;
    uint16_t m_section_idx;
    uint64_t m_value;
    uint64_t m_size;
};

struct SectionHeader
{
    SectionHeader( const ELF_File &ctx, uint64_t header_offset );
    void Dump() const;

    std::string m_name;
    SectionType m_type;
    SectionFlagsBitfield m_attrs;
    uint64_t m_address;
    uint64_t m_offset;
    uint64_t m_size;
    uint32_t m_asso_idx;
    uint32_t m_info;
    uint64_t m_addr_align;
    uint64_t m_ent_size;
};

// High-level structs

struct StringTableSection
{
    std::string m_data;
};

struct Section
{
    std::string m_name;

    std::variant< std::monostate // Unknown/Unset
        , StringTableSection
    > m_var;
};

struct ELF_File
{
    ELF_File( std::string_view file_name, std::vector< unsigned char > &&contents_ );

    uint8_t U8At( uint64_t offset ) const;
    uint16_t U16At( uint64_t offset ) const;
    uint32_t U32At( uint64_t offset ) const;
    uint64_t U64At( uint64_t offset ) const;

    void DumpGroupSection( uint64_t offset, uint64_t size ) const;

    std::vector< unsigned char > contents;
    mutable std::vector< bool > m_read; // Mark all the read bytes

    uint64_t section_header_offset;
    uint16_t section_header_entry_size;
    uint16_t section_header_num_entries;
    uint16_t section_names_header_index;

    std::vector< Section > m_sections;

    std::optional< StringTable > strtab;
    std::optional< StringTable > shstrtab;

    std::vector< uint64_t > section_offsets;
};
