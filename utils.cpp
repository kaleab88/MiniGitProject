#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

// For SHA1
#include <openssl/sha.h>
// For ZLIB
#include <zlib.h>

namespace Utils
{

    std::string sha1(const std::string &input)
    {
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1(reinterpret_cast<const unsigned char *>(input.c_str()), input.length(), hash);
        std::stringstream ss;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    std::string readFile(const std::string &filepath)
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file)
        {
            return ""; // Return empty string if file doesn't exist
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void writeFile(const std::string &filepath, const std::string &content)
    {
        std::ofstream file(filepath, std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Unable to open file for writing: " + filepath);
        }
        file << content;
    }

    void createDirectory(const std::string &path)
    {
        std::filesystem::create_directories(path);
    }
}