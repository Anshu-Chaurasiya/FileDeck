#include <fstream>
#include <iostream>
#include <vector>
#include <string>

std::string main_save_file = "save.bin";

void saveData(std::vector<std::string> data) {
    std::ofstream file(main_save_file);

    if (!file.is_open()) {
        std::cout << "File Could not be accessed\n";
        return;
    }

    for (std::string entry : data) {
        file << entry << '\n';
    }
    std::cout << "Data Saved!\n";
    file.close();
}

std::vector<std::string> fetchData() {
    std::vector<std::string> return_data;
    std::ifstream file(main_save_file);

    if (!file.is_open()) {
        std::cout << "File Could not be accessed\n";
        return return_data;
    }

    std::string line;
    while (std::getline(file, line)) {
        return_data.push_back(line);
    }
    std::cout << "Data Fetched!\n";
    file.close();
    return return_data;
}