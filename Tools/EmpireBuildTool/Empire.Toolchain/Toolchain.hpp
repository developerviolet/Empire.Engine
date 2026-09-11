#pragma once

#include <cstdlib>
#include <iostream>

namespace BuildTool
{
    class Toolchain
    {
    public:

        bool Compile(const std::string& target, const std::string& config);
    };
}