// utils.cpp file
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

// For SHA1
#include <openssl/sha.h>
// For ZLIB (if used, ensure library is linked)
#include <zlib.h> // Keep this if you plan to implement compress/decompress

namespace Utils // Start of the Utils namespace block
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

    // CORRECTED: Removed "Utils::" here because we are already inside the 'namespace Utils' block.
    void writeFile(const std::string &filepath, const std::string &content)
    {
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Error: Utils::writeFile failed to open file for writing: " << filepath << std::endl;
            return;
        }
        file << content;
        file.close();
    }

    // CORRECTED: Removed "Utils::" here for the same reason as writeFile.
    void createDirectory(const std::string &path)
    {
        std::filesystem::create_directories(path);
    }

    // Added basic definitions for compress and decompress to avoid linker errors.
    // You will need to implement their actual logic later.
    std::string compress(const std::string &input)
    {
        // Placeholder implementation
        std::cerr << "Warning: compress function is not fully implemented." << std::endl;
        return input; // Return original for now
    }

    std::string decompress(const std::string &compressed_input)
    {
        // Placeholder implementation
        std::cerr << "Warning: decompress function is not fully implemented." << std::endl;
        return compressed_input; // Return original for now
    }

} // End of Utils namespace block