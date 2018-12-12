#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "elf_structs.hpp"

void RenderAsHTML( std::ostream &html_out, const ELF_File &elf );

std::string escape( const std::string &s );
