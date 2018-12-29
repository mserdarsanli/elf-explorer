#! /usr/bin/python3
#
# Copyright 2018 Mustafa Serdar Sanli
#
# This file is part of ELF Explorer.
#
# ELF Explorer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ELF Explorer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import subprocess

def main():
    parser = argparse.ArgumentParser( description = 'Create object file images.' )
    parser.add_argument( '--out', required = True, help = 'output file name' )
    parser.add_argument( '--label', required = True, help = 'object file name' )
    args = parser.parse_args()

    imagemagick_args = [ 'convert', '-size', '200x200', 'xc:transparent', '-fill', 'white', '-stroke', 'black',
        '-draw', "stroke-linejoin round stroke-width 2  path 'M 60,20 L 120,20 L 140,40 L 140,120 L 60,120 Z'",
        '-draw', "stroke-linejoin round stroke-width 2  path 'M 120,20 L 120,40 L 140,40'",
        '-draw', "stroke-linecap  round stroke-width 5  path 'M 80,50 L 100,50'",
        '-draw', "stroke-linecap  round stroke-width 5  path 'M 80,70 L 120,70'",
        '-draw', "stroke-linecap  round stroke-width 5  path 'M 80,90 L 120,90'",
        '-fill', 'black', '-size', '200x30', '-undercolor', 'white', '-gravity', 'North', '-pointsize', '24', '-annotate', '+0+130', args.label,
        args.out,
    ]

    subprocess.run( imagemagick_args, check = True )

if __name__ == "__main__":
    main()
