#ifndef GNU_COREUTILS
#define GNU_COREUTILS
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

class GNU_coreutils_sort {
        bool numeric = false;
        bool reverse = false;
        bool unique = false;
        std::vector<std::string> in_filenames;
        std::string out_filename;

        void parse_arguments(int argc, const char *argv[]) {
            for (int i{1}; i < argc; ++i) {
                std::string op{argv[i]};
                if (op.front() == '-') {
                    if (op == "-n") numeric = true;
                    else if (op == "-r") reverse = true;
                    else if (op == "-u") unique = true;
                    else if (op == "-o") {
                        if (i + 1 < argc) out_filename = argv[++i];
                        else {
                            std::cout << "The expected output file is missing!" << std::endl;
                            exit(1);
                        }
                    }
                    else {
                        std::cout << "Invalid option!" << std::endl;
                        exit (1);
                    }
                }
                else in_filenames.push_back(op);
            }
        }
    public:
        GNU_coreutils_sort(int argc, const char *argv[]) {
            parse_arguments(argc, argv);
        }

        void sort_clone() {
            std::vector<std::string> data;
            std::string line;
            if (!in_filenames.empty()) {
                for (auto name : in_filenames) {
                    std::ifstream file(name);
                    if (!file.is_open()) {
                        std::cout << "No such file" << std::endl;
                        exit(1);
                    }
                    while (std::getline(file, line)) {
                        data.push_back(line);
                    }
                    file.close();
                }
            }
            else {
                    while (std::getline(std::cin, line)) {
                    data.push_back(line);
                }
            }

            if (data.empty()) return;

            if (numeric) {
                std::sort(data.begin(), data.end(), [](const std::string& str1, const std::string& str2){
                    float num1, num2;
                    try {
                        num1 = std::stof(str1);
                    }
                    catch (std::invalid_argument& e) {
                        num1 = 0;
                    }
                    try {
                        num2 = std::stof(str2);
                    }
                    catch (std::invalid_argument& e) {
                        num2 = 0;
                    }
                    if (num1 == 0 && num2 == 0) {
                        return str1 < str2;
                    }
                    return num1 < num2;
                });
            }
            else {
                std::sort(data.begin(), data.end());
            }
            if (unique) {
                auto it = std::unique(data.begin(), data.end());
                data.erase(it, data.end());
            }
            if (reverse) {
                std::reverse(data.begin(), data.end());
            }
            if (!out_filename.empty()) {
                std::ofstream file(out_filename);
                if (!file.is_open()) {
                    std::cout << "Output file openning error!" << std::endl;
                    exit(1);
                }
                for (auto& st : data) {
                    file << st << std::endl;
                }
                file.close();
            }
            else {
                for (auto& st : data) {
                    std::cout << st << std::endl;
                }
            }
        }
};
#endif

int main(int argc, const char* argv[]) {
    GNU_coreutils_sort obj(argc, argv);
    obj.sort_clone();
}