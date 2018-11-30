#include <iostream>

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

int main()
{
    std::cout << "Main\n";
    return 0;
}
