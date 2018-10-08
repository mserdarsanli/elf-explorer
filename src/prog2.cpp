#include <iostream>

struct GlobalInit
{
    GlobalInit()
    {
        std::cout << "prog2: Constructor for global\n";
    }

    ~GlobalInit()
    {
        std::cout << "prog2: Destructor for global\n";
    }
};

GlobalInit g;

int main()
{
    std::cout << "prog2: Running...\n";

    return 0;
}
