#ifndef FILESCANNER_HPP
#define FILESCANNER_HPP

#include <vector>
#include <string>

class FileScanner {
public:
    FileScanner();
    std::vector<std::tuple<std::string, std::string, std::string>>
        scan_directory(const std::string &directory_path,
                       bool analyze_files, bool analyze_directories);
};

#endif