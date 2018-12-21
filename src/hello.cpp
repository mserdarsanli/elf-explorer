// Copyright 2018 Mustafa Serdar Sanli
//
// This file is part of ELF Explorer.
//
// ELF Explorer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ELF Explorer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ELF Explorer.  If not, see <https://www.gnu.org/licenses/>.


#include <iostream>

#include <cmath>
#include <functional>

struct Foo
{
    Foo()
    {
        std::cout << "Constructing Foo\n";
    }

    ~Foo()
    {
        std::cout << "Destructing Foo\n";
    }
};

Foo f;

void qwe( int a )
{
    if ( a == 3 )
    {
        Foo f;
    }
}

template <typename T>
struct V
{
    static void Foo()
    {
        std::cout << sizeof(T) << "\n";
    }
};

std::function< float( float ) > sqrt_fn = sqrtf;

int main()
{
    std::cout << "Main\n";

    V<int>::Foo();
    V<char>::Foo();

    return 0;
}
