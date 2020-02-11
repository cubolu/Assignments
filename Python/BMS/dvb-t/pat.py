#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - Program Association Table parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    16-10-2019
Testing:    python3.6
"""
import sys

from psi import PSI


class PAT(PSI):
    __PAT_TABLE = 0x00
    __TABLE_EXTENSION_ID = 0x03fd

    def __init__(self, data):
        # parse program-specific information frame
        super().__init__(data)

        if self.table_id != PAT.__PAT_TABLE:
            print("PAT Error:", "Table ID is not PAT ID(0x00).", file=sys.stderr)
        if not self.section_syntax_indicator:
            print("PAT Error:", "Section syntax indicator bit not set to 1.", file=sys.stderr)
        if self.private_bit:
            print("PAT Error:", "Private bit not set to 0.", file=sys.stderr)

        self.__parse_pat_table(self.table_data)

    def __parse_pat_table(self, data):
        position_indicator = 0

        # parse Program Association Table
        self.program_mapping = []
        while position_indicator < len(data):
            # 16 bits program number
            service_id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
            position_indicator += 2
            # 3 bits reserved bits
            reserved = (data[position_indicator] & 0xe0) >> 5
            if reserved != 0x07:
                print("PAT Error: ", "Reserved bits not set to 0x07.", reserved, file=sys.stderr)
            # 13 bits pid
            pid = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big") & 0x1fff
            position_indicator += 2
            self.program_mapping.append((service_id, pid))

    def __str__(self):
        pat_str = super().__str__()
        pat_str += "TS ID: {self.id:#06x}\nProgram mapping (service_id, pid): [".format(self=self) + ", ".join(
            ["(" + ", ".join([format(n, "#06x") for n in mapping]) + ")" for mapping in self.program_mapping]) + "]\n"

        return pat_str
