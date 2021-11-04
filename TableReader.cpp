//
// Created by donla on 04.11.2021.
//

#include "TableReader.h"

void TableReader::setFilePath(char* file_path) { this->file_path = file_path; }

std::vector<std::vector<std::string>>
TableReader::getText() {
    std::ifstream in(file_path);
    size_t N, M;
    in >> N >> M;
    std::vector<std::vector<std::string>> result_matrix;
    result_matrix.resize(N);
    for (auto& it : result_matrix) it.resize(M);

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            getline(in, result_matrix[i][j], ';');
            size_t index = result_matrix[i][j].find('\n');
            if (index < result_matrix[i][j].length()) result_matrix[i][j].erase(index, 1);
        }
    }

    return result_matrix;
}
