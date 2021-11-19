#include <boost/program_options.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <streambuf>

#include "Bitstream.hpp"

using namespace std;
using namespace Tang;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    po::options_description options("Allowed options");
    options.add_options()("help,h", "show help");
    options.add_options()("bin", po::value<std::string>(), "output bin file");
    options.add_options()("bas", po::value<std::string>(), "output bas file");
    options.add_options()("fuse", po::value<std::string>(), "output fuse file");
    options.add_options()("bmk", po::value<std::string>(), "output bmk file");
    options.add_options()("bma", po::value<std::string>(), "output bma file");
    options.add_options()("svf", po::value<std::string>(), "output svf file");
    options.add_options()("verbose", "show parsing info");
    options.add_options()("verbose_data", "show additional parsing data");
    options.add_options()("bits", "extract bits locations");

    po::positional_options_description pos;
    options.add_options()("input", po::value<std::string>()->required(), "input bitstream file");
    pos.add("input", 1);

    po::variables_map vm;
    try {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(options).positional(pos).run();
        po::store(parsed, vm);
        po::notify(vm);
    } catch (std::exception &e) {
        cerr << e.what() << endl << endl;
        goto help;
    }

    if (vm.count("help")) {
    help:
        cerr << "Project Tang - Open Source Tools for Anlogic FPGAs" << endl;
        cerr << "tangbit: Anlogic bitstream converter" << endl;
        cerr << endl;
        cerr << "Copyright (C) 2018 Miodrag Milanovic <mmicko@gmail.com>" << endl;
        cerr << endl;
        cerr << options << endl;
        return vm.count("help") ? 0 : 1;
    }

    bool verbose = vm.count("verbose");

    ifstream bitstream_file(vm["input"].as<string>());
    if (!bitstream_file) {
        cerr << "Failed to open input file" << endl;
        return 1;
    }
    try {
        Bitstream bitstream = Bitstream::read(bitstream_file);
        bitstream.parse(verbose, vm.count("verbose_data"));
        if (verbose)
            printf("Bitstream CRC calculated: 0x%04x\n", (unsigned int)bitstream.calculate_bitstream_crc());
        if (vm.count("fuse")) {
            ofstream fuse_file(vm["fuse"].as<string>(), ios::out | ios::trunc);
            bitstream.write_fuse(fuse_file);
        }
        if (vm.count("bas")) {
            ofstream bas_file(vm["bas"].as<string>(), ios::out | ios::trunc);
            bitstream.write_bas(bas_file);
        }
        if (vm.count("bin")) {
            ofstream bin_file(vm["bin"].as<string>(), ios::out | ios::trunc | ios::binary);
            bitstream.write_bin(bin_file);
        }
        if (vm.count("bma")) {
            ofstream bma_file(vm["bma"].as<string>(), ios::out | ios::trunc);
            bitstream.write_bma(bma_file);
        }
        if (vm.count("bmk")) {
            ofstream bmk_file(vm["bmk"].as<string>(), ios::out | ios::trunc | ios::binary);
            bitstream.write_bmk(bmk_file);
        }
        if (vm.count("svf")) {
            ofstream svf_file(vm["svf"].as<string>(), ios::out | ios::trunc);
            bitstream.write_svf(svf_file);
        }
        if (vm.count("bits")) {
            bitstream.extract_bits();
        }
    } catch (BitstreamParseError e) {
        cerr << e.what() << endl;
    }
    return 0;
}
