//
// Created by donla on 04.11.2021.
//
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>

class TableReader {
public:
    void setFilePath(char* file_path);
    std::vector<std::vector<std::string>> getText();
private:
    char* file_path;
};