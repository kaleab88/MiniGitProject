#include <iostream>
#include <vector>
#include <string>
#include "minigit.h"

void print_usage()
{
    std::cout << "Usage: minigit <command> [<args>]\n"
              << "\n"
              << "Available commands:\n"
              << "  init\t\tCreate an empty MiniGit repository\n"
              << "  add <file>\tAdd file contents to the index\n"
              << "  commit -m <msg>\tRecord changes to the repository\n"
              << "  log\t\tShow the commit logs\n"
              << "  branch <name>\tCreate a new branch\n"
              << "  checkout <branch/commit>\tSwitch branches or restore working tree files\n"
              << "  merge <branch>\tJoin two development histories together\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 1;
    }

    std::string command = argv[1];
    MiniGit mg;

    if (command == "init")
    {
        mg.init();
    }
    else if (command == "add")
    {
        if (argc < 3)
        {
            std::cerr << "Error: No file specified for 'add'." << std::endl;
            return 1;
        }
        mg.add(argv[2]);
    }
    else if (command == "commit")
    {
        if (argc < 4 || std::string(argv[2]) != "-m")
        {
            std::cerr << "Error: Commit message required. Usage: minigit commit -m <message>" << std::endl;
            return 1;
        }
        mg.commit(argv[3]);
    }
    else if (command == "log")
    {
        mg.log();
    }
    else if (command == "branch")
    {
        if (argc < 3)
        {
            std::cerr << "Error: Branch name required." << std::endl;
            return 1;
        }
        mg.branch(argv[2]);
    }
    else if (command == "checkout")
    {
        if (argc < 3)
        {
            std::cerr << "Error: Branch or commit hash required." << std::endl;
            return 1;
        }
        mg.checkout(argv[2]);
    }
    else if (command == "merge")
    {
        if (argc < 3)
        {
            std::cerr << "Error: Branch to merge is required." << std::endl;
            return 1;
        }
        mg.merge(argv[2]);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        print_usage();
        return 1;
    }

    return 0;
}