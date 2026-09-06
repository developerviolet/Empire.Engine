#include "Window.hpp"
#include <iostream>

using namespace Xcb;

int main()
{
    Connection connection;

    
    Window window(connection);
        
    std::cout << "Test\n";

    while (true)
    {
        
    }

    return 0;
}