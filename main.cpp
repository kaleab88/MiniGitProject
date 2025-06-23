#include "minigit.h"
#include <iostream>
#include <vector>
#include <string>
#include <numeric> // For std::accumulate

int main(int argc, char *argv[])
{
    MiniGit mg;

    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.empty())
    {
        std::cout << "Usage: ./minigit <command> [args...]" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  init                      Initialize a new repository." << std::endl;
        std::cout << "  add <filepath>            Add a file to the staging area." << std::endl;
        std::cout << "  commit -m \"<message>\"   Record changes to the repository." << std::endl;
        std::cout << "  log                       Show commit history." << std::endl;
        std::cout << "  branch <branch-name>      Create a new branch." << std::endl;
        std::cout << "  checkout <name>           Switch branches or restore working tree files." << std::endl;
        std::cout << "  merge <branch-name>       Join two or more development histories together." << std::endl;
        return 0;
    }

    std::string command = args[0];

    if (command == "init")
    {
        mg.init();
    }
    else if (command == "add")
    {
        if (args.size() < 2)
        {
            std::cerr << "Usage: ./minigit add <filepath>" << std::endl;
            return 1;
        }
        mg.add(args[1]);
    }
    else if (command == "commit")
    {
        if (args.size() < 3 || args[1] != "-m")
        {
            std::cerr << "Usage: ./minigit commit -m \"<message>\"" << std::endl;
            return 1;
        }
        // Reconstruct the message in case it contains spaces
        std::string message = std::accumulate(args.begin() + 2, args.end(), std::string(),
                                              [](const std::string &a, const std::string &b)
                                              {
                                                  return a.empty() ? b : a + " " + b;
                                              });
        mg.commit(message);
    }
    else if (command == "log")
    {
        mg.log();
    }
    else if (command == "branch")
    {
        if (args.size() < 2)
        {
            std::cerr << "Usage: ./minigit branch <branch-name>" << std::endl;
            return 1;
        }
        mg.branch(args[1]);
    }
    else if (command == "checkout")
    {
        if (args.size() < 2)
        {
            std::cerr << "Usage: ./minigit checkout <branch-name-or-commit-hash>" << std::endl;
            return 1;
        }
        mg.checkout(args[1]);
    }
    else if (command == "merge")
    {
        if (args.size() < 2)
        {
            std::cerr << "Usage: ./minigit merge <branch-name>" << std::endl;
            return 1;
        }
        mg.merge(args[1]);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }

    return 0;
}