#ifndef UTILS_H
#define UTILS_H

#include <string>         // For std::string
#include <vector>         // For std::vector (used in some utility functions)
#include <iostream>       // For std::cerr (used by printErrorAndExit)
#include <cstdlib>        // For exit()
#include <filesystem>     // For filesystem operations

// --- Error handling utilities ---
void printErrorAndExit(const std::string& message);
bool isMiniGitRepo();

// --- Utility class with static methods ---
class Utils {
public:
    // Computes the SHA-1 hash of a given string
    static std::string sha1(const std::string& input);

    // Compresses a string using zlib
    static std::string compress(const std::string& input);

    // Decompresses a string using zlib
    static std::string decompress(const std::string& compressed_input);

    // Reads the entire content of a file into a string
    static std::string readFile(const std::string& filepath);

    // Writes a string to a file (overwrites existing content)
    static void writeFile(const std::string& filepath, const std::string& content);

    // Overload: Writes content to a file using std::filesystem::path
    static void writeFile(const std::filesystem::path& filepath, const std::string& content);

    // Creates a directory if it doesn't exist
    static void createDirectory(const std::string& path);
};

#endif // UTILS_H
