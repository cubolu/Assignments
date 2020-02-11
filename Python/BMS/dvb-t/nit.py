#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - Network Information Table parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    16-10-2019
Testing:    python3.6
"""
import sys

from descriptor import parse_descriptors
from psi import PSI


class NIT(PSI):
    __ACTUAL_NETWORK = 0x40
    __OTHER_NETWORK = 0x41

    def __init__(self, data):
        # parse program-specific information frame
        super().__init__(data)

        if not (self.table_id == NIT.__ACTUAL_NETWORK or self.table_id == NIT.__OTHER_NETWORK):
            print("NIT Error:", "Table ID is not NIT ID(0x40 or 0x41).", file=sys.stderr)
        if not self.section_syntax_indicator:
            print("NIT Error: ", "Section syntax indicator bit not set to 1.",
                  file=sys.stderr)
        if not self.private_bit:
            print("NIT Error:", "Private bit not set to 1.", file=sys.stderr)

        self.__parse_nit_table(self.table_data)

    def __parse_nit_table(self, data):
        position_indicator = 0

        # 4 bits reserved bits
        reserved = (data[position_indicator] & 0xf0) >> 4
        # 12 bits network descriptors length
        network_descriptors_length = int.from_bytes(data[position_indicator:position_indicator + 2],
                                                    byteorder="big") & 0x0fff
        position_indicator += 2
        # variable bits read network descriptors
        self.network_descriptors = parse_descriptors(
            data[position_indicator:position_indicator + network_descriptors_length])
        position_indicator += network_descriptors_length
        # 4 bits reserved bits
        reserved = (data[position_indicator] & 0xf0) >> 4
        # 12 bits transport stream loop length
        ts_loop_length = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big") & 0x0fff
        position_indicator += 2
        # transport stream loop
        self.ts_ids = []
        self.og_network_ids = []
        self.ts_descriptors = []
        ts_loop_end_position = position_indicator + ts_loop_length  # current position + loop length
        while position_indicator < ts_loop_end_position:
            # 16 bits transport stream id
            self.ts_ids.append(int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big"))
            position_indicator += 2
            # 16 bits original delivery system id
            self.og_network_ids.append(int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big"))
            position_indicator += 2
            # 4 bits reserved bits
            reserved = (data[position_indicator] & 0xf0) >> 4
            # 12 bits transport descriptors length
            transport_descriptors_length = int.from_bytes(data[position_indicator:position_indicator + 2],
                                                          byteorder="big") & 0x0fff
            position_indicator += 2
            # variable bits read network descriptors
            self.ts_descriptors.append(
                parse_descriptors(data[position_indicator:position_indicator + transport_descriptors_length]))
            position_indicator += transport_descriptors_length

    def __str__(self):
        nit_str = super().__str__()
        nit_str += "Network ID: {self.id:#x}\n" \
                   "Network descriptors: [".format(self=self) \
                   + ", ".join([format(desc) for desc in self.network_descriptors]) + \
                   "]\n" \
                   "TS IDs: [" \
                   + ", ".join([format(id, "#06x") for id in self.ts_ids]) + \
                   "]\n" \
                   "Original network IDs: [" \
                   + ", ".join([format(id, "#06x") for id in self.og_network_ids]) + \
                   "]\n" \
                   "TS descriptors: [[" \
                   + "], [".join([", ".join([format(d) for d in descs]) for descs in self.ts_descriptors]) + \
                   "]]\n"

        return nit_str
