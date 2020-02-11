#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - program-specific information parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    16-10-2019
Testing:    python3.6
"""
import sys


class PSI:
    def __init__(self, data):
        self.__parse_section(data)
        self.__parse_table(self.__section_data)

    def __parse_section(self, data):
        position_indicator = 0

        # pointer field
        if data[position_indicator] > 0:
            position_indicator = data[position_indicator]
        else:
            position_indicator += 1

        # Table header
        # 8 bits table id
        self.table_id = data[position_indicator]
        position_indicator += 1
        if self.table_id == 0xff:
            print("PSI Info: ", "Table ID is 0xff.", file=sys.stderr)
            return
        # 1 bit section syntax indicator
        self.section_syntax_indicator = (data[position_indicator] & 0x80) != 0
        # 1 bit private bit
        self.private_bit = (data[position_indicator] & 0x40) != 0
        # 2 bits reserved bits
        reserved = (data[position_indicator] & 0x30) >> 4
        if reserved != 0x03:
            print("PSI Error: Table ID", format(self.table_id, "#04x"), "Reserved bits not set to 0x03.",
                  file=sys.stderr)
        # 2 bits section length unused bits
        unused_bits = (data[position_indicator] & 0x0c) >> 2
        if unused_bits != 0x00:
            print("PSI Error: Table ID", format(self.table_id, "#04x"), "Section length unused bits not set to 0.",
                  file=sys.stderr)
        # 10 bits section length
        self.section_length = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big") & 0x03ff
        position_indicator += 2
        if self.section_length > 1021:
            print("PSI Error: Table ID", format(self.table_id, "#04x"), "Section length exceeds a value of 1021",
                  file=sys.stderr)
        elif self.section_length > (len(data) - position_indicator - 4):
            print("PSI Error: Table ID", format(self.table_id, "#04x"), "Section length exceeds a data section",
                  file=sys.stderr)
        # normalise section length: section_length += -crc 4 bytes
        self.section_length -= 4
        # syntax section/table data
        self.__section_data = data[position_indicator:position_indicator + self.section_length]
        position_indicator += self.section_length
        # 32 bits CRC32 - TODO vNext check
        self.crc_32 = data[position_indicator:position_indicator + 4]

    def __parse_table(self, data):
        position_indicator = 0

        # 16 bits id
        self.id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
        position_indicator += 2
        # 2 bit reserved bits
        reserved = (data[position_indicator] & 0xc0) >> 6
        # 5 bits version number
        self.version_number = (data[position_indicator] & 0x3e) >> 1
        # 1 bit current/next indicator
        self.current_next_indicator = data[position_indicator] & 0x01
        position_indicator += 1
        # 8 bits section number indicates which table this is in a related sequence of tables
        self.section_number = data[position_indicator]
        position_indicator += 1
        # 8 bits last section number indicates which table is the last table in the sequence of tables
        self.last_section_number = data[position_indicator]
        position_indicator += 1
        # variable bits table data
        self.table_data = data[position_indicator:]

    def __str__(self):
        psi_str = "Table ID: {self.table_id:#x}\nSection syntax indicator: {self.section_syntax_indicator}\n" \
                  "Private bit: {self.private_bit}\nSection length: {self.section_length} bytes\n" \
                  "Table version: {self.version_number}\nTable section number: {self.section_number}\n" \
                  "Table last section number: {self.last_section_number}\n".format(self=self)

        # psi_str += "Section data: 0x"
        # for byte in self.__section_data:
        #     psi_str += format(byte, "02x")
        # psi_str += "\n"

        return psi_str
