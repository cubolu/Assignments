#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - Service Description Table parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    16-10-2019
Testing:    python3.6
"""
import sys

from descriptor import parse_descriptors
from psi import PSI


class SDT(PSI):
    __ACTUAL_TRANSPORT_STREAM = 0x42
    __OTHER_TRANSPORT_STREAM = 0x46

    def __init__(self, data):
        # parse program-specific information frame
        super().__init__(data)

        # if not (self.table_id == self.id == SDT.__ACTUAL_TRANSPORT_STREAM or self.table_id == SDT.__OTHER_TRANSPORT_STREAM):
        if not (self.table_id == SDT.__ACTUAL_TRANSPORT_STREAM or self.table_id == SDT.__OTHER_TRANSPORT_STREAM):
            print("SDT Error:", "Table ID is not SDT ID(0x42 or 0x46).", file=sys.stderr)
        if not self.section_syntax_indicator:
            print("SDT Error: ", "Section syntax indicator bit not set to 1.",
                  file=sys.stderr)
        if not self.private_bit:
            print("SDT Error:", "Private bit not set to 1.", file=sys.stderr)

        self.__parse_sdt_table(self.table_data)

    def __parse_sdt_table(self, data):
        position_indicator = 0

        # 16 bits original network id
        self.og_network_id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
        position_indicator += 2
        # 8 bits reserved bits
        reserved = data[position_indicator]
        position_indicator += 1
        # transport stream loop
        self.service_ids = []
        self.EIT_schedule_flags = []
        self.EIT_present_following_flags = []
        self.running_status = []
        self.free_CA_mode = []
        self.descriptors = []
        while position_indicator < len(data):
            # 16 bits transport stream id - program numbers from PAT
            self.service_ids.append(int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big"))
            position_indicator += 2
            # 6 bits reserved bits
            reserved = (data[position_indicator] & 0xfc) >> 2
            # 1 bits indicates that EIT schedule information for the service is present
            self.EIT_schedule_flags.append((data[position_indicator] & 0x02) != 0)
            # 1 bits indicates that EIT_present_following information for the service is present
            self.EIT_present_following_flags.append((data[position_indicator] & 0x01) != 0)
            position_indicator += 1
            # 3 bits status of the service
            self.running_status.append((data[position_indicator] & 0xe0) >> 5)
            # 1 bits CA mode
            self.free_CA_mode.append((data[position_indicator] & 0x10) != 0)
            # 12 bits descriptors length
            descriptors_length = int.from_bytes(data[position_indicator:position_indicator + 2],
                                                byteorder="big") & 0x0fff
            position_indicator += 2
            # variable bits read descriptors
            self.descriptors.append(parse_descriptors(data[position_indicator:position_indicator + descriptors_length]))
            position_indicator += descriptors_length

    def __str__(self):
        sdt_str = super().__str__()
        sdt_str += "TS ID: {self.id:#06x}\nOriginal network IDs: {self.og_network_id:#06x}\n" \
                   "Service IDs: [".format(self=self)
        sdt_str += ", ".join([format(id, "#06x") for id in self.service_ids])
        sdt_str += "]\n"
        sdt_str += "TS descriptors: [["
        sdt_str += "], [".join([", ".join([format(d) for d in descs]) for descs in self.descriptors])
        sdt_str += "]]\n"

        return sdt_str
