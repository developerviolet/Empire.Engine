#include "Build.hpp"

#include <cstdlib>
#include <iostream>

namespace BuildTool
{
    bool Build::Run(const std::string& target, const std::string& config)
    {
        std::cout << "Building " << target;

        if (!config.empty())
        {
            std::cout << " (" << config << ")";
        }

        std::cout << "...\n";

        const int result = std::system("echo Compiler will be connected here");

        if (result != 0)
        {
            std::cerr << "Build failed\n";
            return false;
        }

        std::cout << "Build succeeded\n";

        return true;
    }
}