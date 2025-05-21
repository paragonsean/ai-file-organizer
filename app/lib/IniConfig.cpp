#include "IniConfig.hpp"
#include <iostream>


/**
 * Loads the configuration from the specified file.
 *
 * The file should contain a standard INI file format, with sections
 * enclosed in square brackets, and key-value pairs separated by an
 * equals sign. Whitespace is ignored.
 *
 * @param filename the name of the file to load
 * @return true if the file was loaded successfully, false on failure
 */
bool IniConfig::load(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        // New section
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
        }
        // Key-value pair
        else {
            std::istringstream is_line(line);
            std::string key, value;
            if (std::getline(is_line, key, '=') && std::getline(is_line, value)) {
                // Remove whitespace
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                data[section][key] = value;
            }
        }
    }
    return true;
}


/**
 * Retrieves the value of a key from the specified section.
 *
 * If the section does not exist, or the key does not exist within the
 * section, the default value is returned.
 *
 * @param section the section to retrieve the key from
 * @param key the key to retrieve the value for
 * @param default_value the value to return if the key does not exist
 * @return the value of the key, or the default value if it does not exist
 */
std::string IniConfig::getValue(const std::string &section, const std::string &key, const std::string &default_value) const {
    auto sec_it = data.find(section);
    if (sec_it != data.end()) {
        auto key_it = sec_it->second.find(key);
        if (key_it != sec_it->second.end()) {
            return key_it->second;
        }
    }
    return default_value;
}


/**
 * Sets the value for a key within a specified section.
 *
 * If the section or key does not exist, it will be created.
 *
 * @param section the section in which the key-value pair will be set
 * @param key the key for which the value will be set
 * @param value the value to associate with the specified key
 */

void IniConfig::setValue(const std::string &section, const std::string &key, const std::string &value) {
    data[section][key] = value;
}


/**
 * Saves the current configuration to a file.
 *
 * @param filename the name of the file to write the configuration to
 * @return true if the configuration was written successfully, false otherwise
 */
bool IniConfig::save(const std::string &filename) const
{
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return false;
    }

    for (const auto &section : data) {
        file << "[" << section.first << "]\n";
        for (const auto &pair : section.second) {
            file << pair.first << " = " << pair.second << "\n";
        }
        file << "\n";
    }

    return true;
}