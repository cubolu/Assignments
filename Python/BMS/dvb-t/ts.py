#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - internal transport stream representation

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    15-10-2019
Testing:    python3.6
"""
import sys
from collections import defaultdict

from descriptor import Tag, Descriptor
from nit import NIT
from pat import PAT
from pmt import PMT
from sdt import SDT
from ts_packet import TransportPacket


class TransportStream:
    __PAT_PID = 0x0000  # Program Association Table
    __NIT_PID = 0x0010  # Actual Network Table
    __SDT_PID = 0x0011  # Service Description Table

    def __init__(self, file):
        self.raw_packets = defaultdict(list)
        self.packet_count = 0
        self.packet_count_per_pid = defaultdict(int)
        self.programs_specific_pids = {
            self.__PAT_PID,
            self.__NIT_PID,
            self.__SDT_PID
            # PMT PIDs will be added during parsing
        }
        self.pat = defaultdict()
        self.nit = None
        self.sdt = defaultdict()
        self.pmts = defaultdict()

        self.__read(file)

    # API for Transport Stream analysis, returns only first transport stream info (first id in transport stream loop),
    #                                    respectively <NIT>.ts_ids[0], <NIT>.ts_descriptors[0]

    def get_network_name(self):
        return next(
            filter(
                lambda descriptor: descriptor.tag == Tag.NETWORK_NAME_DESCRIPTOR,
                self.nit.network_descriptors)
        ).network_name

    def get_network_id(self):
        return self.nit.id

    def get_bandwidth(self):
        return next(
            filter(
                lambda descriptor: descriptor.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR,
                self.nit.ts_descriptors[0])
        ).bandwidth

    def get_constellation(self):
        return next(
            filter(
                lambda descriptor: descriptor.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR,
                self.nit.ts_descriptors[0])
        ).constellation[0]

    def get_guard_interval(self):
        return next(
            filter(
                lambda descriptor: descriptor.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR,
                self.nit.ts_descriptors[0])
        ).guard_interval[0]

    def get_code_rate(self):
        descriptor = next(filter(lambda descriptor: descriptor.tag == Tag.TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR,
                                 self.nit.ts_descriptors[0]))
        if descriptor.priority == Descriptor.LOW_PRIORITY:
            return descriptor.code_rate_lp_stream[0]
        else:
            return descriptor.code_rate_hp_stream[0]

    def get_programs(self):
        programs = []
        for service_id, descriptors in zip(self.sdt[self.nit.ts_ids[0]].service_ids,
                                           self.sdt[self.nit.ts_ids[0]].descriptors):
            pid = next(
                filter(lambda mapping: mapping[0] == service_id, self.pat[self.nit.ts_ids[0]].program_mapping))[1]
            service_provider = next(
                filter(lambda descriptor: descriptor.tag == Tag.SERVICE_DESCRIPTOR, descriptors)).service_provider
            service_name = next(
                filter(lambda descriptor: descriptor.tag == Tag.SERVICE_DESCRIPTOR, descriptors)).service_name
            bit_rate = self.packets_per_program(pid) / self.packet_count * self.__compute_bit_rate()

            programs.append((pid, service_provider, service_name, bit_rate))

        return programs

    # Parsing Transport Stream:

    def __read(self, file):
        while True:
            packet_payload = file.read(TransportPacket.PACKET_SIZE)

            try:
                if packet_payload:
                    self.packet_count += 1  # including corrupted packets as well

                    packet = TransportPacket(packet_payload)
                    self.__track_stats(packet)
                    self.__parse_packet(packet)

                else:
                    self.__finalize_psi_data_parsing()
                    break
            except ValueError as error:
                print("Packet Error:", error, file=sys.stderr)
            except IndexError as error:
                print("Packet Error:", "Read error.", error, file=sys.stderr)  # packet is most likely corrupted

    def __track_stats(self, packet: TransportPacket):
        self.packet_count_per_pid[packet.pid] += 1

    def __parse_packet(self, packet: TransportPacket):
        if packet.pid in self.programs_specific_pids:
            if packet.pusi == TransportPacket.NEW_PACKET_START:
                # parse previous packet which is now complete
                self.__parse_psi_data(packet.pid)

            self.raw_packets[packet.pid].append(packet.data)

    def __parse_psi_data(self, pid):
        if self.raw_packets[pid]:
            if pid == self.__PAT_PID:
                pat = PAT(b''.join(self.raw_packets[pid]))
                self.__update_programs_specific_pids(pat)
                self.pat[pat.id] = pat
            elif pid == self.__NIT_PID:
                self.nit = NIT(b''.join(self.raw_packets[pid]))
            elif pid == self.__SDT_PID:
                sdt = SDT(b''.join(self.raw_packets[pid]))
                self.sdt[sdt.id] = sdt
            else:  # PMT data
                self.pmts[pid] = PMT(b''.join(self.raw_packets[pid]))

            self.raw_packets[pid] = []

    def __update_programs_specific_pids(self, pat: PAT):
        # PAT mapping: 0x0000 -> NIT PID
        self.__NIT_PID = next(filter(lambda mapping: mapping[0] == 0x0000, pat.program_mapping))[1]

        self.programs_specific_pids = {
            self.__PAT_PID,
            self.__NIT_PID,
            self.__SDT_PID
        }
        # add PMT PIDs
        self.programs_specific_pids.update([mapping[1] for mapping in pat.program_mapping])

    def __finalize_psi_data_parsing(self):
        # process last packets when stream is over, assuming that all data parts were transmitted
        for pid, raw_packets in self.raw_packets.items():
            if raw_packets:
                self.__parse_psi_data(pid)

    def packets_per_program(self, pmt_pid):
        packet_count_per_program = sum(
            [self.packet_count_per_pid[elem_pid] for elem_pid in self.pmts[pmt_pid].elementary_pids]) \
                                   + self.packet_count_per_pid[pmt_pid]  # packets in elementary streams + PMT stream

        return packet_count_per_program

    def __compute_bit_rate(self):
        # COFDM Modulation bit rate
        # https://dveo.com/broadcast/bit-rate-calculation-for-COFDM-modulation.shtml
        a = 54000000 * TransportPacket.PACKET_SIZE / 204
        b = a * self.get_bandwidth() / 8
        c = b * {"QPSK": 1/4, "16-QAM": 2/4, "64-QAM": 3/4, "reserved": 0}[self.get_constellation()]
        d = c * self.get_code_rate()
        guard_interval = self.get_guard_interval()
        e = d * guard_interval.denominator / (guard_interval.numerator + guard_interval.denominator)
        bit_rate = e

        return bit_rate
