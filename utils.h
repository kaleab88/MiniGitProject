#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <filesystem>

class Utils {
public:
    static std::string readFile(const std::string& filepath);
    static void writeFile(const std::string& filepath, const std::string& content);
    static void writeFile(const std::filesystem::path& filepath, const std::string& content); // Overload for fs::path
    static void createDirectory(const std::string& dirpath);
    static std::string sha1(const std::string& data);
};

#endif // UTILS_H