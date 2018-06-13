#include "precompile.hpp"

#include "util/util.hpp"

int main(int argc, char** argv) {
    if (argc != 4)
        throw std::invalid_argument(
            "app-extract-vertex <g2o file> <vertex name> <outputfile>");

    std::string filename = argv[1];
    std::string vertex = argv[2];
    std::string ofilename = argv[3];

    std::ifstream ifile(filename.c_str(), std::ios::in);

    if (!ifile.good())
        throw std::runtime_error("unable to open file");

    std::vector<std::vector<std::string>> vertices;
    std::string current_line;
    while (std::getline(ifile, current_line)) {
        std::vector<std::string> vertex_values;
        if (current_line.find(vertex) == 0) {
            std::stringstream ss(current_line);
            std::string each_word;
            while (std::getline(ss, each_word, ' '))
                vertex_values.push_back(each_word);
            vertices.push_back(vertex_values);
        }
    }

    std::ofstream ofile(ofilename.c_str());
    for (const auto& item : vertices) {
        for (std::size_t i = 2; i < item.size(); ++i) {
            ofile << item[i] << " ";
        }
        ofile << std::endl;
    }

    return 0;
}
