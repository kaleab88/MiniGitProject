#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace Utils
{
    // Computes the SHA-1 hash of a given string
    std::string sha1(const std::string &input);

    // Compresses a string using zlib
    std::string compress(const std::string &input);

    // Decompresses a string using zlib
    std::string decompress(const std::string &compressed_input);

    // Reads the entire content of a file into a string
    std::string readFile(const std::string &filepath);

    // Writes a string to a file, overwriting it
    void writeFile(const std::string &filepath, const std::string &content);

    // Creates a directory if it doesn't exist
    void createDirectory(const std::string &path);
}

#endif // UTILS_H