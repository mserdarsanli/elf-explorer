#include <iostream>

struct GlobalInit
{
    GlobalInit()
    {
        std::cout << "prog1: Constructor for global\n";
    }

    ~GlobalInit()
    {
        std::cout << "prog1: Destructor for global\n";
    }
};

GlobalInit g;

int main()
{
    std::cout << "prog1: Running...\n";

    return 0;
}
