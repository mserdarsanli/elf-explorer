#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

#include "enums.hpp"
#include "input_buffer.hpp"

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

    StringTable( InputBuffer &input, uint64_t section_offset, uint64_t size );
    std::string_view StringAtOffset( uint64_t string_offset ) const;

    std::string m_str;
};

struct Symbol
{
    Symbol( InputBuffer &input, StringTable &strtab, uint64_t offset );

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
    SectionHeader( InputBuffer &input, StringTable &shstrtab, uint64_t header_offset );

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

struct SymbolTable
{
    std::vector< Symbol > m_symbols;
};

struct RelocationEntry
{
    uint64_t m_offset;
    X64RelocationType m_type;
    uint32_t m_symbol;
    int64_t m_addend;
};

struct RelocationEntries
{
    std::vector< RelocationEntry > m_entries;
};

struct GroupSection
{
    uint32_t m_flags;
    std::vector< uint32_t > m_section_indices;
};

struct NoBitsSection
{
    std::string m_data;
};

struct InitArraySection
{
    std::string m_data;
};

struct ProgBitsSection
{
    std::string m_data;
    bool m_is_executable = false; // TODO this can be used from section header
};

struct Section
{
    std::variant< std::monostate
                , GroupSection
                , StringTable
                , SymbolTable
                , RelocationEntries
                , NoBitsSection
                , InitArraySection
                , ProgBitsSection
    > m_var;
};

struct ELF_File
{
    static ELF_File LoadFrom( InputBuffer & );

    std::vector< SectionHeader > m_section_headers;
    std::vector< Section > m_sections;
};
