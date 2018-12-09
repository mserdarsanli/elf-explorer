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
