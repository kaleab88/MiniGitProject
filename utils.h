#ifndef UTILS_H
#define UTILS_H

#include <string>
<<<<<<< HEAD
#include <filesystem>

class Utils {
public:
    static std::string readFile(const std::string& filepath);
    static void writeFile(const std::string& filepath, const std::string& content);
    static void writeFile(const std::filesystem::path& filepath, const std::string& content); // Overload for fs::path
    static void createDirectory(const std::string& dirpath);
    static std::string sha1(const std::string& data);
};
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063

#endif // UTILS_H