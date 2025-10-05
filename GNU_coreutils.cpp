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
                    for (int j{1}; j < op.size(); ++j) {
                        if (op[j] == 'n') numeric = true;
                        else if (op[j] == 'r') reverse = true;
                        else if (op[j] == 'u') unique = true;
                        else if (op[j] == 'o') {
                            if (++i < argc) out_filename = argv[i];
                            else {
                                std::cout << "The expected output file is missing!" << std::endl;
                                exit(1);
                            }
                            break;
                        }
                        else {
                            std::cout << "Invalid option!" << std::endl;
                            exit (1);
                        }
                    }
                }
                else in_filenames.push_back(op);
            }
        }

        std::pair<double, bool> parse_number(const std::string& str) {
            int i{};
            int size = str.size();
            while (i < size && str[i] == ' ') ++i;
            if (i == size) return {0, false};

            bool neg_sign{false};
            if (str[i] == '-') {
                neg_sign = true;
                ++i;
            }
            else if (str[i] == '+') return {0, false};

            std::string tmp;
            while (i < size && (str[i] >= '0' && str[i] <= '9') || str[i] == ',') {
                if (str[i] == ',') ++i;
                else tmp += str[i++];
            }

            if (i < size && str[i] == '.') {
                tmp += '.';
                ++i;
                while (i < size && str[i] >= '0' && str[i] <= '9') tmp += str[i++];
            }

            try {
                auto num = std::stod(tmp);
                return {neg_sign ? -num : num, true};
            }
            catch (std::invalid_argument& e) {
                return {0, false};
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
                std::sort(data.begin(), data.end(), [this](const std::string& str1, const std::string& str2){
                    auto [num1, flag1] = parse_number(str1);
                    auto [num2, flag2] = parse_number(str2);
                    if (!flag1 && !flag2) return str1 < str2;
                    if (flag1 != flag2) {
                        if (flag1 && num1 == 0) return true;
                        else if (flag2 && num2 == 0) return false;
                        return num1 < num2;
                    }
                    if (num1 != num2) return num1 < num2;
                    return str1 < str2;
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