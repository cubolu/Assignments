#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assignment in BMP course - DVB-T transport stream analyser

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    14-10-2019
Testing:    python3.6
"""
from ts import TransportStream


# TODO parse args
# TODO prepare output file name

with open("simple_multiplex.ts", "rb") as input_file:
    ts = TransportStream(input_file)
