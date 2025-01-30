#include "FileScanner.hpp"
#include <iostream>
#include <filesystem>
#include <gtk/gtk.h>

namespace fs = std::filesystem;


FileScanner::FileScanner() {}

std::vector<std::tuple<std::string, std::string, std::string>>
FileScanner::scan_directory(const std::string &directory_path, bool analyze_files, bool analyze_directories)
{
    std::vector<std::tuple<std::string, std::string, std::string>> file_paths_and_names;

    for (const auto &entry : fs::directory_iterator(directory_path)) {
        std::string full_path = entry.path().string();
        std::string name = entry.path().filename().string();
        std::string type;

        if (analyze_files && fs::is_regular_file(entry)) {
            type = "F";
            file_paths_and_names.push_back(std::make_tuple(full_path, name, type));
        } else if (analyze_directories && fs::is_directory(entry)) {
            type = "D";
            file_paths_and_names.push_back(std::make_tuple(full_path, name, type));
        }
    }
    return file_paths_and_names;
}