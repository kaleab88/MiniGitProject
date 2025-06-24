#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>      // For std::hex, std::setw, std::setfill
#include <filesystem>   // For std::filesystem operations
#include <stdexcept>    // Good practice for potential exceptions

// For SHA-1 hashing with OpenSSL
#include <openssl/sha.h>
#include <openssl/evp.h> // Potentially for more robust OpenSSL usage
#include <openssl/err.h> // For OpenSSL error handling

// For ZLIB compression/decompression (needed for compress/decompress functions)
#include <zlib.h>

namespace fs = std::filesystem; // Alias for std::filesystem

// --- Freestanding Error Handling and Repository Check Functions ---
// These are declared in utils.h and provide project-wide error handling.
void printErrorAndExit(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    exit(1); // Exit with a non-zero status to indicate an error
}

bool isMiniGitRepo() {
    // Check if the current directory or any parent directory is a MiniGit repo
    fs::path current_path = fs::current_path();
    while (true) {
        if (fs::exists(current_path / ".minigit") && fs::is_directory(current_path / ".minigit")) {
            return true;
        }
        if (current_path.has_parent_path() && current_path.parent_path() != current_path) {
            current_path = current_path.parent_path();
        } else {
            break; // Reached root directory
        }
    }
    return false;
}

// --- Utils Class Static Method Implementations ---

// Reads the entire content of a file into a string
std::string Utils::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary); // Use binary mode for consistent reading
    if (!file.is_open()) {
        // For readFile, it's often better to return empty string or throw if file doesn't exist/can't be opened
        // printErrorAndExit("Could not open file for reading: " + filepath); // Or handle more gracefully
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close(); // Explicitly close the file
    return buffer.str();
}

// Writes a string to a file, creating parent directories if necessary
void Utils::writeFile(const std::string& filepath, const std::string& content) {
    fs::path p(filepath);
    if (p.has_parent_path() && !fs::exists(p.parent_path())) {
        try {
            fs::create_directories(p.parent_path());
        } catch (const fs::filesystem_error& e) {
            printErrorAndExit("Could not create parent directories for file: " + filepath + " - " + e.what());
        }
    }
    std::ofstream file(filepath, std::ios::binary); // Use binary mode for consistent writing
    if (!file.is_open()) {
        printErrorAndExit("Could not open file for writing: " + filepath);
    }
    file << content;
    file.close(); // Explicitly close the file
}

// Overload for std::filesystem::path
void Utils::writeFile(const std::filesystem::path& filepath, const std::string& content) {
    if (filepath.has_parent_path() && !fs::exists(filepath.parent_path())) {
        try {
            fs::create_directories(filepath.parent_path());
        } catch (const fs::filesystem_error& e) {
            printErrorAndExit("Could not create parent directories for file: " + filepath.string() + " - " + e.what());
        }
    }
    std::ofstream file(filepath.string(), std::ios::binary); // Use binary mode and convert path to string
    if (!file.is_open()) {
        printErrorAndExit("Could not open file for writing (fs::path): " + filepath.string());
    }
    file << content;
    file.close(); // Explicitly close the file
}

// Creates a directory if it doesn't exist, including parent directories
void Utils::createDirectory(const std::string& dirpath) {
    if (!fs::exists(dirpath)) {
        try {
            fs::create_directories(dirpath);
        } catch (const fs::filesystem_error& e) {
            printErrorAndExit("Could not create directory: " + dirpath + " - " + e.what());
        }
    }
}

// Computes the SHA-1 hash of a given string
std::string Utils::sha1(const std::string& data) {
    unsigned char hash[SHA_DIGEST_LENGTH]; // 20 bytes for SHA-1
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

// Compresses a string using zlib (placeholder implementation)
std::string Utils::compress(const std::string& input) {
    // This is a placeholder. A full implementation would use zlib functions like compress2 or deflate.
    // Example:
    // uLongf compressed_size = compressBound(input.length());
    // std::vector<Bytef> compressed_data(compressed_size);
    // int result = compress2(compressed_data.data(), &compressed_size,
    //                        reinterpret_cast<const Bytef*>(input.data()), input.length(), Z_BEST_COMPRESSION);
    // if (result != Z_OK) { /* handle error */ }
    // return std::string(reinterpret_cast<char*>(compressed_data.data()), compressed_size);

    std::cerr << "Warning: Utils::compress function is a placeholder and not fully implemented." << std::endl;
    return input; // For now, return original string
}

// Decompresses a string using zlib (placeholder implementation)
std::string Utils::decompress(const std::string& compressed_input) {
    // This is a placeholder. A full implementation would use zlib functions like uncompress or inflate.
    std::cerr << "Warning: Utils::decompress function is a placeholder and not fully implemented." << std::endl;
    return compressed_input; // For now, return original string
}