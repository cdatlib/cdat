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


#include "Index.hpp"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace cdat;
namespace po = boost::program_options;


Index * load_from_file(std::string const &file_name) {
    std::filebuf fb;
    fb.open (file_name, std::ios::in);
    std::istream in(&fb);

    Index *index = Index::load_index(in);
    fb.close();

    return index;
}

void print_locate(std::string const &pattern, std::vector<ulong> const &occ, std::ostream &out) {
    out << "\'" <<pattern << "\' occurrences are:\n" << "[";

    for (size_t i = 0; i < occ.size(); ++i) {
        out << occ[i];

        if (i < occ.size() - 1) {
            out << ", ";
        }
    }

    out << "]\n";
}

void locate(Index * index, std::string &filename, std::ostream &out) {
    timeval start, stop, t2;
    unsigned long time = 0;

    gettimeofday(&start, NULL);
    std::ifstream file(filename);
    std::string line;
    ulong count_global = 0;

    while (getline(file, line)) {
        ulong count = 0;
        std::vector<ulong> occurrences;
        occurrences.reserve(1 << 10);

        index->locate(line, line.size(), &occurrences, &count);
        print_locate(line, occurrences, out);

        count_global += count;

    }

    file.close();

    gettimeofday(&stop, NULL);
    timersub(&stop, &start, &t2);
    time += (t2.tv_sec) * 1000 + (t2.tv_usec) / 1000;

    std::cout << "Located " << count_global << " occurrences of patterns in " << time / 1000.0 << "[s].\n";
}

void count(Index * index, std::string &filename, std::ostream &out) {
    timeval start, stop, t2;
    unsigned long time = 0;

    gettimeofday(&start, NULL);
    std::ifstream file(filename);
    std::string line;
    ulong global_count = 0;

    while (getline(file, line)) {
        ulong count = 0;
        index->count(line, line.size(), &count);
        out << "\'" << line << "\' number of occurrences = " << count << "\n";

        global_count += count;
    }

    file.close();

    gettimeofday(&stop, NULL);
    timersub(&stop, &start, &t2);
    time += (t2.tv_sec) * 1000 + (t2.tv_usec) / 1000;

    std::cout << "Found " << global_count << " occurrences of pattern in " << time / 1000.0 << "[s].\n";
}

int main(int argc, char *argv[]) {
    std::string input_file;
    std::string pattern_file;
    std::string output_file;
    bool isLocate = false;
    bool save_file = false;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce a help message")
            ("index,i", po::value<std::string>(&input_file)->required(), "input file with index")
            ("pattern,p", po::value<std::string>(&pattern_file)->required(), "file with patterns to search")
            ("out,o", po::value<std::string>(&output_file), "file to write results, if not specified results will be displayed to stdout.")
            ("action,a", po::value<std::string>(&pattern_file)->required(), "locate or count")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc;
            return 0;
        }

        po::notify(vm);

        {
            std::string action = vm["action"].as< std::string >();
            if (action == "locate") {
                isLocate = true;
            }
            else if (action != "count") {
                std::cerr << "Wrong action type, available options are locate and count\n";
                return -1;
            }
        }
        if (vm.count("out")) {
            save_file = true;
        }
    }
    catch (std::exception& e) {
        std::cout << e.what() << "\n";
        return 0;
    }

    try {
        Index *index = load_from_file(input_file);
        std::cout << "Loaded " << index->get_size_in_mega_bytes() << "[mb] into memory.\n";
        if (save_file) {
            std::filebuf fb;
            fb.open(output_file, std::ios::out);
            std::ostream out(&fb);

            if (isLocate) {
                locate(index, pattern_file, out);
            }
            else {
                count(index, pattern_file, out);
            }

            fb.close();
        }
        else {
            if (isLocate) {
                locate(index, pattern_file, std::cout);
            }
            else {
                count(index, pattern_file, std::cout);
            }
        }
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << "\n";
        return -1;
    }

    return 0;
}
