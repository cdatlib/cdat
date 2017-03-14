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

char replace(char const letter) {
    switch (letter) {
        case 'R':
            return rand() % 2 ? 'G' : 'A';
        case 'Y':
            return rand() % 2 ? 'T' : 'C';
        case 'K':
            return rand() % 2 ? 'G' : 'T';
        case 'M':
            return rand() % 2 ? 'A' : 'C';
        case 'S':
            return rand() % 2 ? 'G' : 'C';
        case 'W':
            return rand() % 2 ? 'A' : 'T';
        case 'B':
            return rand() % 3 == 0 ? 'G' : rand() % 2 ? 'T' : 'C';
        case 'D':
            return rand() % 3 == 0 ? 'G' : rand() % 2 ? 'A' : 'T';
        case 'H':
            return rand() % 3 == 0 ? 'A' : rand() % 2 ? 'C' : 'T';
        case 'V':
            return rand() % 3 == 0 ? 'G' : rand() % 2 ? 'C' : 'A';
        case 'N':
        default:
            return rand() % 4 == 0 ? 'T' : rand() % 3 == 0 ? 'G' : rand() % 2 ? 'C' : 'A';
    }
}

int main(int argc, char *argv[]) {
    std::string input_file;
    std::string output_file;

    try {
        po::options_description desc("Replaces characters in text with \'A\', \'C\', \'G\', \'T\'.\n\nAllowed options");
        desc.add_options()
            ("help,h", "produce a help message")
            ("in,i", po::value<std::string>(&input_file)->required(), "input file")
            ("out,o", po::value<std::string>(&output_file)->required(), "file to write results.")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc;
            return 0;
        }

        po::notify(vm);
    }
    catch (std::exception& e) {
        std::cout << e.what() << "\n";
        return 0;
    }

    srand(time(NULL));
    ifstream file(input_file.c_str());

    const size_t BUFFER_SIZE = 16 * 1024;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    std::filebuf fb;
    fb.open(output_file.c_str(), std::ios::out);
    std::ostream out(&fb);
    do {
        file.read(buffer, BUFFER_SIZE);
        bytes_read = (size_t) file.gcount();

        for (size_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] != 'A' && buffer[i] != 'C' && buffer[i] != 'G' && buffer[i] != 'T') {
                buffer[i] = replace(buffer[i]);
            }
        }

        out.write(buffer, bytes_read);
    }
    while (bytes_read == BUFFER_SIZE);

    fb.close();
    file.close();
    return 0;
}
