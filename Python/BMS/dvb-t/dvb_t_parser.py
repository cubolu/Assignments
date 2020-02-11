#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - DVB-T transport stream analyser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    14-10-2019
Testing:    python3.6
"""
import argparse
from pathlib import Path

from ts import TransportStream


def main():
    # parse input file name as an command line argument
    parser = argparse.ArgumentParser(description="DVB-T transport stream analyser.", add_help=False)
    parser.add_argument("input_file", type=argparse.FileType("rb"), metavar="FILE.ts", help="input transport stream")
    args = parser.parse_args()

    with args.input_file as input_file, open(Path(args.input_file.name).with_suffix(".txt"), "w") as output_file:
        ts = TransportStream(input_file)
        output_file.write("Network name: {}\n".format(ts.get_network_name()))
        output_file.write("Network ID: {}\n".format(ts.get_network_id()))
        output_file.write("Bandwidth: {} MHz\n".format(ts.get_bandwidth()))
        output_file.write("Constellation: {}\n".format(ts.get_constellation()))
        output_file.write("Guard interval: {}\n".format(ts.get_guard_interval()))
        output_file.write("Code rate: {}\n\n".format(ts.get_code_rate()))
        programs = ts.get_programs()  # program: (pid, service_provider, service_name, bitrate)
        programs.sort(key=lambda program: program[0])
        for program in programs:
            output_file.write(
                "{:#06x}-{}-{}: {:.2f} Mbps\n".format(program[0], program[1], program[2], program[3] / 1000000))


if __name__ == '__main__':
    main()
