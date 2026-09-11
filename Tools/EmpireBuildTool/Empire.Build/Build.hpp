#pragma once

#include <string>

namespace BuildTool
{
    class Build
    {
    public:
        bool Run(const std::string& target, const std::string& config);
    };
}