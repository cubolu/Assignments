#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - Descriptor parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    18-10-2019
Testing:    python3.6
"""
import sys
from enum import IntEnum
from fractions import Fraction


def parse_descriptors(data):
    if not data:
        return []

    # data argument may include multiple descriptors
    descriptors = [Descriptor(data)]
    while descriptors[-1].get_unparsed_data():
        descriptors.append(Descriptor(descriptors[-1].get_unparsed_data()))

    return descriptors


class Tag(IntEnum):
    NETWORK_NAME_DESCRIPTOR = 0x40
    SERVICE_DESCRIPTOR = 0x48
    LINKAGE_DESCRIPTOR = 0x4a
    TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR = 0x5a


class Descriptor:
    LOW_PRIORITY = 0
    HIGH_PRIORITY = 1

    def __init__(self, data):
        self.tag = None
        self.__unparsed_data = None
        self.__parse_descriptor(data)

    def get_unparsed_data(self):
        return self.__unparsed_data

    def __parse_descriptor(self, data):
        # 8 bits descriptor tag
        self.tag = data[0]
        # 8 bits descriptor length
        length = data[1]

        descriptor_data = data[2:2 + length]
        self.__unparsed_data = data[2 + length:]
        if self.tag == Tag.NETWORK_NAME_DESCRIPTOR:
            self.network_name = self.__convert_to_string(descriptor_data)
        elif self.tag == Tag.SERVICE_DESCRIPTOR:
            self.__parse_service_descriptor(descriptor_data)
        elif self.tag == Tag.LINKAGE_DESCRIPTOR:
            self.__parse_linkage_descriptor(descriptor_data)  # implemented but not needed for the analysis
        elif self.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR:
            self.__parse_terrestrial_delivery_system_descriptor(descriptor_data)
        # elif self.tag == ...:
        #     pass
        else:
            pass
            # print("Descriptor Error:", "Tag {tag:#04x} not supported.".format(tag=self.tag), file=sys.stderr)

    def __parse_service_descriptor(self, data):
        position_indicator = 0

        # 8 bits service type
        self.service_type = data[position_indicator]  # coded according to Table 87 of standard
        position_indicator += 1
        # 8 bits service provider name length
        service_provider_name_length = data[position_indicator]
        position_indicator += 1
        # variable bits service provider name
        self.service_provider = self.__convert_to_string(
            data[position_indicator:position_indicator + service_provider_name_length])
        position_indicator += service_provider_name_length
        # 8 bits service name length
        service_name_length = data[position_indicator]
        position_indicator += 1
        # variable bits service name
        self.service_name = self.__convert_to_string(data[position_indicator:position_indicator + service_name_length])

    def __parse_linkage_descriptor(self, data):
        position_indicator = 0

        # 16 bits transport stream id
        self.transport_stream_id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
        position_indicator += 2
        # 16 bits original network id
        self.original_network_id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
        position_indicator += 2
        # 16 bits uniquely identifies an information service within a transport stream
        self.service_id = int.from_bytes(data[position_indicator:position_indicator + 2], byteorder="big")
        position_indicator += 2
        # 8 bits linkage type
        self.linkage_type = data[position_indicator]  # coded according to Table 58 of standard
        position_indicator += 1
        # optional fields - skip for now
        if self.linkage_type == 0x08:
            position_indicator += 3  # mobile_hand_over_info() fixed length of 3 bytes
        elif self.linkage_type == 0x0D:
            position_indicator += 3  # event_linkage_info() fixed length of 3 bytes
        elif self.linkage_type == 0x0E:
            extended_event_linkage_info_loop_length = data[position_indicator]
            position_indicator += 1 + extended_event_linkage_info_loop_length  # extended_event_linkage_info() length byte and loop
        # variable bits private data
        private_data = data[position_indicator:]

        # additional private data parse - TS 102 006 Table 1
        if self.linkage_type == 0x09:
            self.__parse_linkage_descriptor_0x09(private_data)

    def __parse_linkage_descriptor_0x09(self, data):
        position_indicator = 0

        # 8 bits data length
        oui_data_length = data[position_indicator]
        position_indicator += 1
        self.oui = []
        self.oui_selectors = []
        oui_data_end_position = position_indicator + oui_data_length  # current position + loop length
        while position_indicator < oui_data_end_position:
            # 24 bits specific organization identification (OUI)
            self.oui.append(int.from_bytes(data[position_indicator:position_indicator + 3], byteorder="big"))
            position_indicator += 3
            # 8 bits selector length
            selector_length = data[position_indicator]
            position_indicator += 1
            # variable bits selector
            self.oui_selectors.append(data[position_indicator:position_indicator + selector_length])
        # variable bits private data
        private_data = data[position_indicator:]

    def __parse_terrestrial_delivery_system_descriptor(self, data):
        # length of descriptor is set - position indicator not needed

        if len(data) != 11:
            print("Descriptor Error:",
                  "Tag {tag:#04x} Descriptor length do not match its structure.".format(tag=self.tag), file=sys.stderr)
        # 32 bits centre frequency value in multiples of 10 Hz
        self.centre_frequency = int.from_bytes(data[0:0 + 4], byteorder="big") * 10  # Hz
        # 3 bits bandwidth in MHz
        self.bandwidth = self.__convert_bandwidth((data[4] & 0b11100000) >> 5)  # MHz
        # 1 bits stream's hierarchical priority (0 = "LP", 1 = "HP")
        self.priority = (data[4] & 0b00010000) != 0
        # 1 bits time slicing indicator
        self.time_slicing_indicator = (data[4] & 0b00001000) != 0
        # 1 bits MPE-FEC indicator
        self.mpe_fec_indicator = (data[4] & 0b00000100) != 0
        # 2 bits reserved bits
        reserved = data[4] & 0b00000011
        # 2 bits constellation pattern used on a terrestrial delivery system
        self.constellation = self.__convert_constellation((data[5] & 0b11000000) >> 6)
        # 3 bits hierarchy information
        self.hierarchy_information = (data[5] & 0b00111000) >> 3
        # 3 bits code rate high priority stream
        self.code_rate_hp_stream = self.__convert_code_rate(data[5] & 0b00000111)
        # 3 bits code rate low priority stream
        self.code_rate_lp_stream = self.__convert_code_rate((data[6] & 0b11100000) >> 5)
        # 2 bits guard interval
        self.guard_interval = self.__convert_guard_interval((data[6] & 0b00011000) >> 3)
        # 2 bits transmission mode
        self.transmission_mode = self.__convert_transmission_mode((data[6] & 0b00000110) >> 1)
        # 1 bits other frequency in use
        self.other_frequency_flag = (data[6] & 0b00000001) != 0
        # 32 bits reserved bits
        reserved = int.from_bytes(data[6:6 + 4], byteorder="big")

    def __str__(self):
        if self.tag == Tag.NETWORK_NAME_DESCRIPTOR:
            desc_str = "{{" \
                       "tag: {self.tag:#04x}, " \
                       "network_name: {self.network_name}" \
                       "}}".format(self=self)
        elif self.tag == Tag.SERVICE_DESCRIPTOR:
            desc_str = "{{" \
                       "tag: {self.tag:#04x}, " \
                       "service_type: {self.service_type}, " \
                       "service_provider: {self.service_provider}, " \
                       "service_name: {self.service_name}" \
                       "}}".format(self=self)
        elif self.tag == Tag.LINKAGE_DESCRIPTOR:
            desc_str = "{{" \
                       "tag: {self.tag:#04x}, " \
                       "transport_stream_id: {self.transport_stream_id:#06x}, " \
                       "original_network_id: {self.original_network_id:#06x}, " \
                       "service_id: {self.service_id:#06x}, " \
                       "linkage_type: {self.linkage_type:#04x}".format(self=self)
            if self.linkage_type == 0x09:
                desc_str += ", service_id: {self.service_id:#06x}, " \
                            "oui: {self.oui}, " \
                            "oui_selectors: {self.oui_selectors}".format(self=self)
            desc_str += "}"
        elif self.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR:
            desc_str = "{{" \
                       "tag: {self.tag:#04x}, " \
                       "centre_frequency: {self.centre_frequency} Hz, " \
                       "bandwidth: {self.bandwidth} MHz, " \
                       "priority: {priority}, " \
                       "time_slicing_indicator: {self.time_slicing_indicator}, " \
                       "mpe_fec_indicator: {self.mpe_fec_indicator}, " \
                       "constellation: {self.constellation}, " \
                       "hierarchy_information: {self.hierarchy_information}, " \
                       "code_rate_hp_stream: {self.code_rate_hp_stream[0]}, " \
                       "code_rate_lp_stream: {self.code_rate_lp_stream[0]}, " \
                       "guard_interval: {self.guard_interval[0]}, " \
                       "transmission_mode: {self.transmission_mode[0]}, " \
                       "other_frequency_flag: {self.other_frequency_flag}" \
                       "}}".format(self=self, priority=("LP", "HP")[self.priority == Descriptor.HIGH_PRIORITY])
        else:
            desc_str = "{{tag: {self.tag:#04x}, data: Not implemented}}".format(self=self)
        return desc_str

    @staticmethod
    def __convert_to_string(raw_chars):
        string_descriptor = ""
        for char in raw_chars:
            string_descriptor += chr(char)

        return string_descriptor

    @staticmethod
    def __convert_bandwidth(bandwidth_code):
        if bandwidth_code == 0b000:
            return 8
        elif bandwidth_code == 0b001:
            return 7
        elif bandwidth_code == 0b010:
            return 6
        elif bandwidth_code == 0b011:
            return 5
        else:
            print("Descriptor Error:", "Terrestrial delivery system descriptor bandwidth not valid.", file=sys.stderr)
            return 0

    @staticmethod
    def __convert_constellation(constellation_code):
        if constellation_code == 0b00:
            return "QPSK", 0b00
        elif constellation_code == 0b01:
            return "16-QAM", 0b01
        elif constellation_code == 0b10:
            return "64-QAM", 0b10
        else:
            print("Descriptor Error:", "Terrestrial delivery system descriptor constellation not valid.",
                  file=sys.stderr)
            return "reserved", 0b11

    @staticmethod
    def __convert_code_rate(code_rate_code):
        if code_rate_code == 0b000:
            return Fraction(1, 2), 0b000
        elif code_rate_code == 0b001:
            return Fraction(2, 3), 0b001
        elif code_rate_code == 0b010:
            return Fraction(3, 4), 0b010
        elif code_rate_code == 0b011:
            return Fraction(5, 6), 0b011
        elif code_rate_code == 0b100:
            return Fraction(7, 8), 0b100
        else:
            print("Descriptor Error:", "Terrestrial delivery system descriptor code rate not valid.", file=sys.stderr)
            return 0

    @staticmethod
    def __convert_guard_interval(guard_interval_code):
        if guard_interval_code == 0b00:
            return Fraction(1, 32), 0b00
        elif guard_interval_code == 0b01:
            return Fraction(1, 16), 0b01
        elif guard_interval_code == 0b10:
            return Fraction(1, 8), 0b10
        else:
            return Fraction(1, 4), 0b11

    @staticmethod
    def __convert_transmission_mode(transmission_mode_code):
        if transmission_mode_code == 0b00:
            return "2k mode", 0b00
        elif transmission_mode_code == 0b01:
            return "8k mode", 0b01
        elif transmission_mode_code == 0b10:
            return "4k mode", 0b10
        else:
            print("Descriptor Error:", "Terrestrial delivery system descriptor transmission mode not valid.",
                  file=sys.stderr)
            return "reserved", 0b11
