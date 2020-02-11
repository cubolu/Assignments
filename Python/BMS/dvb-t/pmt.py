#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - Program Mapping Table parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    22-10-2019
Testing:    python3.6
"""
import sys

from descriptor import parse_descriptors
from psi import PSI


class PMT(PSI):
    def __init__(self, data):
        # parse program-specific information frame
        super().__init__(data)

        if not self.section_syntax_indicator:
            print("PMT Error: Program", format(self.id, "#06x"), "Section syntax indicator bit not set to 1.",
                  file=sys.stderr)
        if self.private_bit:
            print("PMT Error: Program", format(self.id, "#06x"), "Private bit not set to 0.", file=sys.stderr)

        self.__parse_pmt_table(self.table_data)

    def __parse_pmt_table(self, data):
        position_indicator = 0

        # 3 bits reserved bits
        reserved = (data[position_indicator] & 0xe0) >> 5
        if reserved != 0x07:
            print("PMT Error: Program", format(self.id, "#06x"), "Reserved bits not set to 0x07.",
                  file=sys.stderr)
        # 13 bits PCR PID
        self.pcr_pid = int.from_bytes(data[position_indicator:position_indicator + 2],
                                      byteorder="big") & 0x1fff
        position_indicator += 2
        # 4 bits reserved bits
        reserved = (data[position_indicator] & 0xf0) >> 4
        if reserved != 0x0f:
            print("PMT Error: Program", format(self.id, "#06x"), "Reserved bits not set to 0x0f.",
                  file=sys.stderr)
        # 2 bits program info length unused bits
        unused_bits = (data[position_indicator] & 0x0c) >> 2
        if unused_bits != 0x00:
            print("PMT Error: Program", format(self.id, "#06x"), "Section length unused bits not set to 0.",
                  file=sys.stderr)
        # 10 bits program info length
        program_info_length = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big") & 0x03ff
        position_indicator += 2
        # variable bits read program descriptors
        self.program_descriptors = parse_descriptors(
            data[position_indicator:position_indicator + program_info_length])
        position_indicator += program_info_length

        # elementary stream info loop
        self.stream_type = []
        self.elementary_pids = []
        self.elementary_descriptors = []
        while position_indicator < len(data):
            # 8 bits elementary stream type
            self.stream_type.append(data[position_indicator])
            position_indicator += 1
            # 3 bits reserved bits
            reserved = (data[position_indicator] & 0xe0) >> 5
            if reserved != 0x07:
                print("PMT Error: Program", format(self.id, "#06x"), "Reserved bits not set to 0x07.", file=sys.stderr)
            # 13 bits elementary stream pid
            self.elementary_pids.append(int.from_bytes(data[position_indicator:position_indicator + 2],
                                                       byteorder="big") & 0x1fff)
            position_indicator += 2
            # 4 bits reserved bits
            reserved = (data[position_indicator] & 0xf0) >> 4
            if reserved != 0x0f:
                print("PMT Error: Program", format(self.id, "#06x"), "Reserved bits not set to 0x0f.",
                      file=sys.stderr)
            # 2 bits program info length unused bits
            unused_bits = (data[position_indicator] & 0x0c) >> 2
            if unused_bits != 0x00:
                print("PMT Error: Program", format(self.id, "#06x"), "Section length unused bits not set to 0.",
                      file=sys.stderr)
            # 10 bits elementary stream info length
            elementary_info_length = int.from_bytes(data[position_indicator:position_indicator + 2],
                                                    byteorder="big") & 0x03ff
            position_indicator += 2
            # variable bits read descriptors
            self.elementary_descriptors.append(
                parse_descriptors(data[position_indicator:position_indicator + elementary_info_length]))
            position_indicator += elementary_info_length

    def __str__(self):
        pmt_str = super().__str__()
        pmt_str += "Program number: {self.id:#06x}\n" \
                   "Stream type: [".format(self=self) \
                   + ", ".join([format(id, "#04x") for id in self.stream_type]) + \
                   "]\n" \
                   "ES PIDs: [" \
                   + ", ".join([format(id, "#06x") for id in self.elementary_pids]) + \
                   "]\n"

        return pmt_str
