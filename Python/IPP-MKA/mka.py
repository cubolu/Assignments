#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Assigmnet in IPP course - DFA minimization

Author:     Jakub Lukac
E-mail:     xlukac09@stud.fit.vutbr.cz
Created:    06-04-2016
Testing:    python3.4.4
"""

import sys
import argparse

from fsa import *


# Argument Parser with custom error function
class ArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        self.print_usage(sys.stderr)
        print('{0}: error: {1}'.format(self.prog, message), file=sys.stderr)

        exc = sys.exc_info()[1]  # get ArgumentError object
        if exc:
            if exc.args[0].dest == 'input':
                exit(2)
            elif exc.args[0].dest == 'output':
                exit(3)

        exit(1)


# parse arguments using an ArgumentParser object
parser = ArgumentParser(description='DFA minimization', add_help=False)
oper = parser.add_mutually_exclusive_group()

# define arguments
parser.add_argument('--help', action='store_true',
                    help='show this help message and exit')
parser.add_argument('--input', type=argparse.FileType('r'), default=sys.stdin, metavar='FILE',
                    help='(default: stdin)')
parser.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout, metavar='FILE',
                    help='(default: stdout)')
oper.add_argument('-f', '--find-non-finishing', action='store_true', dest='find_non_fin',
                  help='find non finishing state, only one is possible otherwise return "0"')
oper.add_argument('-m', '--minimize', action='store_true',
                  help='minimize DFA')
parser.add_argument('-i', '--case-insensitive', action='store_true', dest='icase',
                    help='states names and symbols are case insensitive')
# BONUS WHT
parser.add_argument('-w', '--white-char', action='store_true', dest='white_delim',
                    help='white char as separator')
# BONUS MST
oper.add_argument('--analyze-string', dest='analyze',
                  help='analyze string as input for DFA')

args = parser.parse_args()

# print help
if args.help:
    parser.print_help()
    if args.find_non_fin or args.minimize or args.icase or args.white_delim:
        exit(1)
    exit(0)

fsa_string = ''

# read FSA from input file
for line in args.input:
    fsa_string += line.split('#', 1)[0] + ' '

# normalize FSA input
fsa_string = fsa_string.strip()
if args.icase:
    fsa_string = fsa_string.lower()

fsa = Fsa(args.white_delim)

try:
    fsa.parse(fsa_string)
except Exception as err:
    msg, code = err.args
    print('{prog}:'.format(prog=sys.argv[0]), 'error:', msg, file=sys.stderr)
    exit(code)

valid, msg = fsa.isvalid_fsa()
if not valid:
    print('{prog}:'.format(prog=sys.argv[0]), 'error:', msg, file=sys.stderr)
    exit(62)

if args.find_non_fin:
    # output non-finishing state
    if fsa.non_terminating_state:
        args.output.write(fsa.non_terminating_state)
    else:
        args.output.write('0')
elif args.minimize:
    # minimize FSA using algorithm from course Formal Languages and Compilers
    fsa.minimization()
    fsa.write(file=args.output)
elif args.analyze:
    fsa.minimization()
    result = fsa.analyze_str(args.analyze)
    if result < 0:
        print('{prog}:'.format(prog=sys.argv[0]), 'error: String', args.analyze,
              'include some unknown symbols', file=sys.stderr)
        exit(1)
    elif result == 0:
        args.output.write('0')
    else:
        args.output.write('1')
else:
    # no option - just print normalized FSA
    fsa.write(file=args.output)

exit(0)
