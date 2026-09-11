#include "Empire.CLI/CLI.hpp"

int main(int argc, char** argv)
{
    BuildTool::CLI cli(argc, argv);

    return cli.Run() ? 0 : 1;
}