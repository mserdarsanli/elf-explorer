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

int main()
{
    std::cout << "Main\n";

    V<int>::Foo();
    V<char>::Foo();

    return 0;
}
