let enum_info = {

    // Enum values for SectionType

    SHT_NULL: "Section header table entry unused",
    SHT_PROGBITS: "Program data",
    SHT_SYMTAB: "Symbol table",
    SHT_STRTAB: "String table",
    SHT_RELA: "Relocation entries with addends",
    SHT_HASH: "Symbol hash table",
    SHT_DYNAMIC: "Dynamic linking information",
    SHT_NOTE: "Notes",
    SHT_NOBITS: "Program space with no data (bss)",
    SHT_REL: "Relocation entries, no addends",
    SHT_SHLIB: "Reserved",
    SHT_DYNSYM: "Dynamic linker symbol table",
    SHT_INIT_ARRAY: "Array of constructors",
    SHT_FINI_ARRAY: "Array of destructors",
    SHT_PREINIT_ARRAY: "Array of pre-constructors",
    SHT_GROUP: "Section group",
    SHT_SYMTAB_SHNDX: "Extended section indeces",
    SHT_NUM: "Number of defined types",

    // Enum values for SymbolBinding

    STB_LOCAL: "Local symbol",
    STB_GLOBAL: "Global symbol",
    STB_WEAK: "Weak symbol",

    // Enum values for SymbolType

    STT_NOTYPE: "Symbol type is unspecified",
    STT_OBJECT: "Symbol is a data object",
    STT_FUNC: "Symbol is a code object",
    STT_SECTION: "Symbol associated with a section",
    STT_FILE: "Symbol's name is file name",
    STT_COMMON: "Symbol is a common data object",
    STT_TLS: "Symbol is thread-local data object",
    STT_NUM: "Number of defined types",

    // Enum values for SymbolVisibility

    STV_DEFAULT: "Default symbol visibility rules",
    STV_INTERNAL: "Processor specific hidden class",
    STV_HIDDEN: "Sym unavailable in other modules",
    STV_PROTECTED: "Not preemptible, not exported",

    // Enum values for X64RelocationType

    R_X86_64_NONE: "No reloc",
    R_X86_64_64: "Direct 64 bit",
    R_X86_64_PC32: "PC relative 32 bit signed",
    R_X86_64_GOT32: "32 bit GOT entry",
    R_X86_64_PLT32: "32 bit PLT address",
    R_X86_64_COPY: "Copy symbol at runtime",
    R_X86_64_GLOB_DAT: "Create GOT entry",
    R_X86_64_JUMP_SLOT: "Create PLT entry",
    R_X86_64_RELATIVE: "Adjust by program base",
    R_X86_64_GOTPCREL: "32 bit signed PC relative offset to GOT",
    R_X86_64_32: "Direct 32 bit zero extended",
    R_X86_64_32S: "Direct 32 bit sign extended",
    R_X86_64_16: "Direct 16 bit zero extended",
    R_X86_64_PC16: "16 bit sign extended pc relative",
    R_X86_64_8: "Direct 8 bit sign extended",
    R_X86_64_PC8: "8 bit sign extended pc relative",
    R_X86_64_DTPMOD64: "ID of module containing symbol",
    R_X86_64_DTPOFF64: "Offset in module's TLS block",
    R_X86_64_TPOFF64: "Offset in initial TLS block",
    R_X86_64_TLSGD: "32 bit signed PC relative offset to two GOT entries for GD symbol",
    R_X86_64_TLSLD: "32 bit signed PC relative offset to two GOT entries for LD symbol",
    R_X86_64_DTPOFF32: "Offset in TLS block",
    R_X86_64_GOTTPOFF: "32 bit signed PC relative offset to GOT entry for IE symbol",
    R_X86_64_TPOFF32: "Offset in initial TLS block",
    R_X86_64_PC64: "PC relative 64 bit",
    R_X86_64_GOTOFF64: "64 bit offset to GOT",
    R_X86_64_GOTPC32: "32 bit signed pc relative offset to GOT",
    R_X86_64_GOT64: "64-bit GOT entry offset",
    R_X86_64_GOTPCREL64: "64-bit PC relative offset to GOT entry",
    R_X86_64_GOTPC64: "64-bit PC relative offset to GOT",
    R_X86_64_GOTPLT64: "like GOT64, says PLT entry needed",
    R_X86_64_PLTOFF64: "64-bit GOT relative offset to PLT entry",
    R_X86_64_SIZE32: "Size of symbol plus 32-bit addend",
    R_X86_64_SIZE64: "Size of symbol plus 64-bit addend",
    R_X86_64_GOTPC32_TLSDESC: "GOT offset for TLS descriptor.",
    R_X86_64_TLSDESC_CALL: "Marker for call through TLS descriptor.",
    R_X86_64_TLSDESC: "TLS descriptor.",
    R_X86_64_IRELATIVE: "Adjust indirectly by program base",
    R_X86_64_RELATIVE64: "64-bit adjust by program base",
    R_X86_64_PC32_BND: "Reserved",
    R_X86_64_PLT32_BND: "Reserved",
    R_X86_64_GOTPCRELX: "Load from 32 bit signed pc relative offset to GOT entry without REX prefix, relaxable.",
    R_X86_64_REX_GOTPCRELX: "Load from 32 bit signed pc relative offset to GOT entry with REX prefix, relaxable.",
    R_X86_64_NUM: "NO_DATA",

    // Enum values for SectionFlags

    SHF_WRITE: "Writable",
    SHF_ALLOC: "Occupies memory during execution",
    SHF_EXECINSTR: "Executable",
    SHF_MERGE: "Might be merged",
    SHF_STRINGS: "Contains nul-terminated strings",
    SHF_INFO_LINK: "`sh_info` contains SHT index",
    SHF_LINK_ORDER: "Preserve order after combining",
    SHF_OS_NONCONFORMING: "Non-standard OS specific handling required",
    SHF_GROUP: "Section is member of a group",
    SHF_TLS: "Section hold thread-local data",
    SHF_COMPRESSED: "Section with compressed data",

    // Enum values for GroupHandling

    GRP_COMDAT: "Mark group as COMDAT",
};
