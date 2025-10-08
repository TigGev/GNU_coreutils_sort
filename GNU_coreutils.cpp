#ifndef GNU_COREUTILS
#define GNU_COREUTILS
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>
#include <queue>
#include <filesystem>
#include <memory>
#include <fstream>

class GNU_coreutils_sort {
        bool numeric{false};
        bool reverse{false};
        bool unique{false};
        std::vector<std::string> in_filenames;
        std::string out_filename;
        int chunk_count{0};

        void parse_arguments(int argc, const char *argv[]) {
            if (argc < 2) {
                std::cout << "Usage: " << argv[0] << " [-nru] [-o output_file] [input_files...]" << std::endl;
                exit(1);
            }
            for (int i{1}; i < argc; ++i) {
                std::string op{argv[i]};
                if (op.front() == '-') {
                    for (int j{1}; j < op.size(); ++j) {
                        if (op[j] == 'n') numeric = true;
                        else if (op[j] == 'r') reverse = true;
                        else if (op[j] == 'u') unique = true;
                        else if (op[j] == 'o') {
                            if (j + 1 < op.size()) {
                                out_filename = op.substr(j + 1);
                            }
                            else {
                                if (++i < argc) {
                                    out_filename = argv[i];
                                }
                                else {
                                    std::cout << "The expected output file is missing!" << std::endl;
                                    exit(1);
                                }
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
            while (i < size && ((str[i] >= '0' && str[i] <= '9') || str[i] == ',')) {
                if (str[i] == ',') ++i;
                else tmp += str[i++];
            }

            if (i < size && str[i] == '.') {
                tmp += '.';
                ++i;
                while (i < size && ((str[i] >= '0' && str[i] <= '9') || str[i] == ',')) {
                    if (str[i] == ',') ++i;
                    else tmp += str[i++];
                }
            }

            try {
                auto num = std::stod(tmp);
                return {neg_sign ? -num : num, true};
            }
            catch (std::invalid_argument& e) {
                return {0, false};
            }
        }

        void solution(std::optional<std::vector<std::unique_ptr<std::ifstream>>>& input_files, std::ostream& output) { //// mi qani faylic kardalu logika
            int chunk_size{1000};
            std::vector<std::string> data;
            data.reserve(chunk_size);
            std::vector<std::fstream> files;
            std::string line;

            auto comparator = [this](const std::string& str1, const std::string& str2){
                if (numeric) {
                    auto [num1, flag1] = this->parse_number(str1);
                    auto [num2, flag2] = this->parse_number(str2);
                    if (!flag1 && !flag2) return reverse ? str2 < str1 : str1 < str2;
                    if (flag1 != flag2) {
                        if (flag1 && num1 == 0) return reverse ? false : true;
                        else if (flag2 && num2 == 0) return reverse ? true : false;
                        return reverse ? num2 < num1 : num1 < num2;
                    }
                    if (num1 != num2) return reverse ? num2 < num1 : num1 < num2;
                    return reverse ? str2 < str1 : str1 < str2;
                }
                else return reverse ? str2 < str1 : str1 < str2;
            };

            auto sort_and_write_to_chunkfile = [&](){
                std::sort(data.begin(), data.end(), comparator);
                if (unique) {
                    auto it = std::unique(data.begin(), data.end());
                    data.erase(it, data.end());
                }
                std::string filename("chunk_file");
                filename += std::to_string(chunk_count++) + ".txt";
                std::fstream chunk_file(filename, std::ios::in | std::ios::out | std::ios::trunc);
                if (!chunk_file.is_open()) {
                    std::cout << "File openning error!" << std::endl;
                    exit(1);
                }
                for (auto& str : data) chunk_file << str << "\n";
                chunk_file.seekg(0);
                chunk_file.clear();
                files.push_back(std::move(chunk_file));
                data.clear();
            };

            if(input_files.has_value()) {
                for (auto& file : *input_files) {
                    while (std::getline(*file, line)) {
                        data.push_back(std::move(line));
                        line.clear();
                        if (data.size() == chunk_size) {
                            sort_and_write_to_chunkfile();
                        }
                    }
                    file->close();
                }
            }
            else {
                while (std::getline(std::cin, line)) {
                    data.push_back(std::move(line));
                    line.clear();
                    if (data.size() == chunk_size) {
                        sort_and_write_to_chunkfile();
                    }
                }
            }

            if (!data.empty()) {
                sort_and_write_to_chunkfile();
            }

            auto neg_comparator = [&](const std::pair<std::string, int>& p1, const std::pair<std::string, int>& p2) {
                return comparator(p1.first, p2.first);
            };

            std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, decltype(neg_comparator)> pq(neg_comparator);
            for (int i{}; i < files.size(); ++i) {
                std::string line;
                if (std::getline(files[i], line)) pq.push({std::move(line), i});
            }



            while (!pq.empty()) {
                auto p = pq.top();
                pq.pop();
                output << p.first << '\n'; 

                std::string line;
                if (std::getline(files[p.second], line)) {
                    pq.push({std::move(line), p.second});
                }
                else {
                    files[p.second].close();
                    std::string name("chunk_file");
                    name += std::to_string(p.second) + ".txt";
                    std::filesystem::remove(name);
                }
            }
        }   

    public:
        GNU_coreutils_sort(int argc, const char *argv[]) {
            parse_arguments(argc, argv);
        }

        void sort_clone() {
            std::vector<std::string> data;
            std::string line;
            std::optional<std::vector<std::unique_ptr<std::ifstream>>> files{};

            
            if (!in_filenames.empty()) {
                files.emplace();
                for (auto name : in_filenames) {
                    auto file = std::make_unique<std::ifstream>(name);
                    if (!file->is_open()) {
                        std::cout << "No such file" << std::endl;
                        exit(1);
                    }
                    files->push_back(std::move(file));
                }
            }
            
            if (!out_filename.empty() && out_filename != "-") {
                std::ofstream file(out_filename);
                if (!file.is_open()) {
                    std::cout << "Output file openning error!" << std::endl;
                    exit(1);
                }               
                solution(files, file);
                file.close();
            }
            else {
                solution(files, std::cout);
            }
        }
};

int main(int argc, const char* argv[]) {
    GNU_coreutils_sort obj(argc, argv);
    obj.sort_clone();
}
#endif