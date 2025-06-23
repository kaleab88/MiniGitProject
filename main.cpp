<<<<<<< HEAD
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
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063

    if (command == "init")
    {
        mg.init();
    }
    else if (command == "add")
    {
<<<<<<< HEAD
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
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    }
    else if (command == "log")
    {
        mg.log();
    }
    else if (command == "branch")
    {
<<<<<<< HEAD
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
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
<<<<<<< HEAD
=======
        print_usage();
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
        return 1;
    }

    return 0;
}