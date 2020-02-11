#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - transport stream packet parser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    14-10-2019
Testing:    python3.6
"""


class TransportPacket:
    PACKET_SIZE = 188
    NEW_PACKET_START = True
    PREVIOUS_PACKET_CONTINUES = False

    def __init__(self, payload):
        # header size is 4 bytes
        self.__parse_header(payload[:4])
        adaptation_field_control_size = 0
        if self.adaptation_field_control == 2 or self.adaptation_field_control == 3:
            # 5th packet byte is size of adaptation field
            adaptation_field_control_size = payload[4] + 1  # including 5th byte itself
            self.__parse_adaptation_field(payload[4:4 + adaptation_field_control_size])

        if self.adaptation_field_control == 1 or self.adaptation_field_control == 3:
            self.data = payload[4 + adaptation_field_control_size:]

    def __parse_header(self, raw_header):
        # 8 bits check sync byte
        if raw_header[0] != 0x47:
            raise ValueError("Sync byte do not match.")

        # 1 bits transport error indicator
        self.tei = (raw_header[1] & 0x80) != 0
        if self.tei:
            raise ValueError("The packet is corrupt.")

        # 1 bits payload unit start indicator
        self.pusi = (raw_header[1] & 0x40) != 0 # NEW_PACKET_START or PREVIOUS_PACKET_CONTINUES

        # 1 bits transport priority
        self.tp = (raw_header[1] & 0x20) != 0

        # 13 bits pid
        self.pid = int.from_bytes(raw_header[1:3], byteorder="big") & 0x1fff

        # 2 bits transport scrambling control
        self.tsc = (raw_header[3] & 0xc0) >> 6

        # 2 bits adaptation field control
        self.adaptation_field_control = (raw_header[3] & 0x30) >> 4  # TODO vNext convert to enum

        # 4 bits continuity counter
        self.counter = (raw_header[3] & 0x0f)

    def __parse_adaptation_field(self, raw_adaptation_field):
        pass

    def __str__(self):
        return "TEI: {self.tei}\nPUSI: {self.pusi}\nTP: {self.tp}\nPID: {self.pid:#06x}\n" \
               "TSC: {self.tsc:#04b}\nAdaptation field control: {self.adaptation_field_control:#04b}\n" \
               "Counter: {self.counter:#x}\n".format(self=self)
