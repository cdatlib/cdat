/* cdat - compressed direct-address tables

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/


#include <fstream>
#include <iostream>
#include <stdlib.h>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {

    std::string input_file;
    std::string output_file;
    bool save_file = false;
    unsigned int number, pattern_length;

    try {
        po::options_description desc("Generates patterns from provided text.\n\nAllowed options");
        desc.add_options()
            ("help,h", "produce a help message")
            ("in,i", po::value<std::string>(&input_file)->required(), "input file")
            ("out,o", po::value<std::string>(&output_file)->required(), "file to write results.")
            ("n,n", po::value<unsigned int>(&number)->required(), "number of patterns")
            ("length,l", po::value<unsigned int>(&pattern_length)->required(), "pattern length")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc;
            return 0;
        }

        po::notify(vm);
        if (vm.count("out")) {
            save_file = true;
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << "\n";
        return 0;
    }

    ifstream file(input_file.c_str());

    file.seekg(0, ios::end);
    unsigned int text_length = file.tellg();

    file.clear();
    file.seekg(0, ios::beg);

    text_length -= pattern_length + 1;
    srand(time(NULL));

    vector<int> positions;
    positions.reserve(number);
    for (int i = 0; i < number; ++i) {
        positions.push_back(rand() % text_length);
    }
    sort(positions.begin(), positions.end());

    std::filebuf fb;
    fb.open(output_file.c_str(), std::ios::out);
    std::ostream out(&fb);
    char * buffer = new char[pattern_length + 1];
    buffer[pattern_length] = '\0';
    int last = 0;
    for (int i = 0; i < number; ++i) {
        file.seekg(positions[i] - last, ios::cur);
        file.read(buffer, pattern_length);
        out << buffer << endl;
        last = positions[i] + pattern_length;
    }

    file.close();
    fb.close();
    return 0;
}
