#pragma once

#include <string>

namespace BuildTool
{
    class CLI
    {
    public:
        CLI(int argc, char** argv);

        bool Run();

    private:
        int argc;
        char** argv;

        std::string target;
        std::string config;
    };
}