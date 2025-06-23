<<<<<<< HEAD
=======
// utils.cpp file
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
<<<<<<< HEAD
#include <iomanip> // For std::hex, std::setw, std::setfill
#include <filesystem>

// For SHA-1 hashing with OpenSSL
#include <openssl/sha.h>
#include <openssl/evp.h> // For EVP_MD_CTX, etc. if needed for more complex hashing
#include <openssl/err.h> // For error handling

namespace fs = std::filesystem;

std::string Utils::readFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        // std::cerr << "Warning: Could not open file for reading: " << filepath << std::endl;
        return ""; // Return empty string on failure
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Utils::writeFile(const std::string &filepath, const std::string &content)
{
    fs::path p(filepath);
    if (p.has_parent_path() && !fs::exists(p.parent_path()))
    {
        fs::create_directories(p.parent_path());
    }
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing: " << filepath << std::endl;
        return;
    }
    file << content;
}

void Utils::writeFile(const std::filesystem::path &filepath, const std::string &content)
{
    // Ensure parent directories exist
    if (filepath.has_parent_path() && !fs::exists(filepath.parent_path()))
    {
        fs::create_directories(filepath.parent_path());
    }
    std::ofstream file(filepath.string());
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing (fs::path): " << filepath.string() << std::endl;
        return;
    }
    file << content;
}

void Utils::createDirectory(const std::string &dirpath)
{
    if (!fs::exists(dirpath))
    {
        if (!fs::create_directories(dirpath))
        {
            std::cerr << "Error: Could not create directory: " << dirpath << std::endl;
        }
    }
}

// Function to compute SHA-1 hash of a string
std::string Utils::sha1(const std::string &data)
{
    unsigned char hash[SHA_DIGEST_LENGTH]; // 20 bytes for SHA-1
    SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}
=======
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
>>>>>>> 4f8bf363fc285507403972efdc9ff5c491a62063
