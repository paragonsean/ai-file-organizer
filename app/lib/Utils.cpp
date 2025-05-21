#include "Utils.hpp"
#include <filesystem>
#include <stdlib.h>
#include <string>
#include <glibmm/fileutils.h>
#ifdef _WIN32
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <limits.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
    #include <limits.h>
#endif
#include <iostream>


/**
 * Checks if the network is available by attempting to ping a known server.
 *
 * This function sends a single ping request to "google.com" and checks the result.
 * It uses system-specific commands to perform the ping operation:
 * - On Windows, it uses "ping -n 1".
 * - On Unix-like systems (Linux, macOS), it uses "ping -c 1".
 *
 * @return true if the ping is successful (network is available), false otherwise.
 */

bool Utils::is_network_available()
{
#ifdef _WIN32
    int result = system("ping -n 1 google.com > NUL 2>&1");
#else
    int result = system("ping -c 1 google.com > /dev/null 2>&1");
#endif
    return result == 0;
}


/**
 * Retrieves the full path of the current executable.
 *
 * This function uses platform-specific methods to determine the absolute path
 * of the current executable:
 * - On Windows, it uses the GetModuleFileName function.
 * - On Linux, it uses the readlink() function on "/proc/self/exe".
 * - On macOS, it uses the _NSGetExecutablePath() function.
 *
 * @return A string containing the full path of the current executable.
 *
 * @exception std::runtime_error If the platform is not supported or if there
 * is an error determining the path.
 */
std::string Utils::get_executable_path()
{
#ifdef _WIN32
    char result[MAX_PATH];
    GetModuleFileName(NULL, result, MAX_PATH);
    return std::string(result);
#elif __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return (count != -1) ? std::string(result, count) : "";
#elif __APPLE__
    char result[PATH_MAX];
    uint32_t size = sizeof(result);
    if (_NSGetExecutablePath(result, &size) == 0) {
        return std::string(result);
    } else {
        throw std::runtime_error("Path buffer too small");
    }
#else
    throw std::runtime_error("Unsupported platform");
#endif
}


/**
 * @brief Checks if the specified path is a valid directory.
 *
 * This function uses the GFileTest library to test if the given path
 * is a directory. It returns true if the path is a valid directory and
 * false otherwise.
 *
 * @param path The path to check for validity as a directory.
 *
 * @return True if the path is a valid directory, false otherwise.
 */

bool Utils::is_valid_directory(const char *path)
{
    return g_file_test(path, G_FILE_TEST_IS_DIR);
}


/**
 * Converts a string of hexadecimal characters to a vector of unsigned char.
 *
 * The function takes a string of hexadecimal characters and interprets them
 * as a sequence of bytes. It then returns a vector of unsigned char
 * containing the bytes.
 *
 * @param hex The string to convert.
 *
 * @return A vector of unsigned char containing the bytes of the input
 * string.
 */
std::vector<unsigned char> Utils::hex_to_vector(const std::string& hex) {
    std::vector<unsigned char> data;
    for (size_t i = 0; i < hex.length(); i += 2) {
        unsigned char byte = static_cast<unsigned char>(std::stoi(hex.substr(i, 2), nullptr, 16));
        data.push_back(byte);
    }
    return data;
}


/**
 * @brief Converts a std::u8string to a const char*.
 *
 * This function simply casts the result of std::u8string::c_str()
 * to a const char*. This is necessary because the C++ standard does
 * not guarantee that the result of std::u8string::c_str() will be a
 * pointer to a null-terminated array of char. This function provides
 * a safe way to get a const char* from a std::u8string.
 *
 * @param u8str The std::u8string to convert.
 *
 * @return A const char* pointing to the contents of the input string.
 */
const char* Utils::to_cstr(const std::u8string& u8str) {
    return reinterpret_cast<const char*>(u8str.c_str());
}


/**
 * Ensures that the specified directory exists.
 *
 * This function checks if the given directory path exists. If it does not,
 * it attempts to create the directory. If the creation of the directory
 * fails for any reason, this function will throw a std::exception.
 *
 * @param dir The directory path to ensure exists.
 */
void Utils::ensure_directory_exists(const std::string &dir)
{
    try {
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    } catch (const std::exception &e) {
        std::cerr << "Error creating log directory: " << e.what() << std::endl;
        throw;
    }
}