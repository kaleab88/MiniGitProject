// --- RESOLVED main.cpp ---

#include "minigit.h" // Includes MiniGit class and its methods
#include "utils.h"   // Includes printErrorAndExit and isMiniGitRepo
#include <iostream>
#include <vector>
#include <string>
#include <numeric>   // For std::accumulate (used for reconstructing commit messages)
#include <filesystem> // For std::filesystem::path (used by isMiniGitRepo indirectly)


// Helper function to print usage instructions
void print_usage()
{
    std::cout << "Usage: minigit <command> [args...]\n"
              << "\n"
              << "Available commands:\n"
              << "  init                      Initialize a new repository.\n"
              << "  add <filepath>            Add a file to the staging area.\n"
              << "  commit -m \"<message>\"   Record changes to the repository.\n"
              << "  log                       Show commit history.\n"
              << "  branch <branch-name>      Create a new branch.\n"
              << "  checkout <name>           Switch branches or restore working tree files.\n"
              << "  merge <branch-name>       Join two or more development histories together.\n";
    // Add Diff Viewer usage if you implement the optional bonus later
    // std::cout << "  diff <commit1> <commit2>  Show line-by-line differences between commits.\n";
}

// Function to check if repository is initialized (moved here for command argument validation)
// This is a duplicate of isMiniGitRepo in utils, but sometimes useful for direct main.cpp checks
// However, it's better to use the one from utils.h/cpp directly.
// Let's rely on utils.h's isMiniGitRepo for consistency.

int main(int argc, char* argv[])
{
    // Check for minimum arguments (just 'minigit' itself)
    if (argc < 2)
    {
        print_usage();
        return 1; // Exit with error
    }

    std::string command = argv[1];
    MiniGit mg; // Initialize MiniGit object

    // Create a vector of arguments, excluding the program name (argv[0])
    // The command itself (e.g., "init", "add") is args[0] in this vector.
    // The actual command arguments start from args[1].
    std::vector<std::string> args(argv + 1, argv + argc);

    if (command == "init")
    {
        // Init command doesn't require repo to be initialized
        if (args.size() != 1) { // Only expects "minigit init"
            printErrorAndExit("Invalid usage. Usage: minigit init");
        }
        mg.init();
    }
    else
    {
        // For all other commands, check if the MiniGit repository is initialized
        if (!isMiniGitRepo()) {
            printErrorAndExit("Not a MiniGit repository. Run 'minigit init' first.");
        }

        if (command == "add")
        {
            if (args.size() < 2) // Expects "minigit add <filepath>"
            {
                printErrorAndExit("Invalid usage. Usage: minigit add <filepath>");
            }
            // If you want to allow multiple files, you would loop through args from index 1
            // For now, assuming single file as per original prompt
            mg.add(args[1]);
        }
        else if (command == "commit")
        {
            // Expects "minigit commit -m <message>"
            if (args.size() < 3 || args[1] != "-m")
            {
                printErrorAndExit("Invalid usage. Usage: minigit commit -m \"<message>\"");
            }
            // Reconstruct the message in case it contains spaces
            std::string message = std::accumulate(args.begin() + 2, args.end(), std::string(),
                                                  [](const std::string& a, const std::string& b) {
                                                      return a.empty() ? b : a + " " + b;
                                                  });
            if (message.empty()) {
                printErrorAndExit("Commit message cannot be empty.");
            }
            mg.commit(message);
        }
        else if (command == "log")
        {
            if (args.size() != 1) // Expects "minigit log"
            {
                printErrorAndExit("Invalid usage. Usage: minigit log");
            }
            mg.log();
        }
        else if (command == "branch")
        {
            if (args.size() < 2) // Expects "minigit branch <branch-name>"
            {
                printErrorAndExit("Invalid usage. Usage: minigit branch <branch-name>");
            }
            // Additional validation for branch name can be done in MiniGit::branch
            mg.branch(args[1]);
        }
        else if (command == "checkout")
        {
            if (args.size() < 2) // Expects "minigit checkout <name>"
            {
                printErrorAndExit("Invalid usage. Usage: minigit checkout <branch-name-or-commit-hash>");
            }
            // Additional validation for name can be done in MiniGit::checkout
            mg.checkout(args[1]);
        }
        else if (command == "merge")
        {
            if (args.size() < 2) // Expects "minigit merge <branch-name>"
            {
                printErrorAndExit("Invalid usage. Usage: minigit merge <branch-name>");
            }
            // Additional validation for branch name can be done in MiniGit::merge
            mg.merge(args[1]);
        }
        // --- Add 'else if' for Diff Viewer here if you implement it later ---
        /*
        else if (command == "diff") {
            if (args.size() < 3 || args.size() > 3) { // Expects "minigit diff <commit1> <commit2>"
                printErrorAndExit("Invalid usage. Usage: minigit diff <commit1> <commit2>");
            }
            mg.diff(args[1], args[2]);
        }
        */
        else // Catch-all for unknown commands after init check
        {
            printErrorAndExit("Unknown command: '" + command + "'");
        }
    }

    return 0; // Success
}