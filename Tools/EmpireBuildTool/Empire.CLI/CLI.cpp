#include "CLI.hpp"

#include "../Empire.Build/Build.hpp"

namespace BuildTool
{
    CLI::CLI(int argc, char** argv)
        : argc(argc), argv(argv)
    {
    }

    bool CLI::Run()
    {
        if (argc < 2)
        {
            return false;
        }

        target = argv[1];

        for (int i = 2; i < argc; ++i)
        {
            std::string argument = argv[i];

            if (argument == "--config" && i + 1 < argc)
            {
                config = argv[++i];
            }
        }

        Build build;

        return build.Run(target, config);
    }
}