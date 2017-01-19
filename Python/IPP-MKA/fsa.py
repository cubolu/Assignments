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
import re


class Fsa(object):
    """ADT for finite-state automaton"""

    def __init__(self, white_delimiter=False):
        self.states = []
        self.alphabet = []
        self.transitions = []
        self.init_state = None
        self.final_states = []
        self.non_terminating_state = None

        # FSA syntax
        if white_delimiter:
            self.delim_re = r'(?:\s*,\s*|\s+)'
            self.sym_re = r"('(?:''|[^']{0,1})'|[^'\s,]{0,1})"
        else:
            self.delim_re = r'\s*,\s*'
            self.sym_re = r"'(''|[^']{0,1})'"

        self.id_re = r'[^\W\d_](?:[\w]*[^\W_]|)'

        self.white_delimiter = white_delimiter

    def parse(self, raw_fsa):
        """Convert FSA from string to object representation.

        Args:
            raw_fsa (str): properly specified FSA from input.
        Raises:
            Exception: if raw_fsa is not a syntactically correct.
            ValueError: if state or symbol of raw_fsa is invalid.

        """

        fsa_re = r'\(\s*' \
                 r'{{(?P<states>.*)}}' \
                 r'{delim}' \
                 r'{{(?P<alphabet>.*)}}' \
                 r'{delim}' \
                 r'{{(?P<rules>.*)}}' \
                 r'{delim}' \
                 r'(?P<init>{id})' \
                 r'{delim}' \
                 r'{{(?P<final>.*)}}' \
                 r'\s*\)'.format(delim=self.delim_re, id=self.id_re)
        result = re.fullmatch(fsa_re, raw_fsa, flags=re.UNICODE | re.DOTALL)

        if result is None:
            # raw_fsa not match proper syntax
            raise Exception('Invalid FSA, probably missing some part of its definition', 60)

        self.states = self.__split_states(result.group('states'))

        self.alphabet = self.__split_alph(result.group('alphabet'))

        self.transitions = self.__parse_transitions(result.group('rules'))

        self.init_state = result.group('init')
        if self.init_state not in self.states:
            # initial state must be one of defined states
            raise ValueError('Initial state is not in set of states', 61)

        self.final_states = self.__split_states(result.group('final'))
        for state in self.final_states:
            if state not in self.states:
                raise ValueError('Set of final states is not a subset of all states', 61)

    def __split_states(self, states):
        states = states.strip()
        states_re = r'(?:{id}' \
                    r'(?:{delim}{id})*){{0,1}}'.format(delim=self.delim_re, id=self.id_re)

        if re.fullmatch(states_re, states, flags=re.UNICODE) is None:
            raise Exception('Invalid set of states', 60)

        # return list of unique states
        return list(set(re.findall(self.id_re, states, flags=re.UNICODE)))

    def __split_alph(self, alphabet):
        alphabet = alphabet.strip()
        if not alphabet:
            raise ValueError('Empty alphabet', 61)

        alphabet_re = r"{sym}" \
                      r"(?:{delim}{sym})*".format(delim=self.delim_re, sym=self.sym_re)

        if re.fullmatch(alphabet_re, alphabet, flags=re.UNICODE | re.DOTALL) is None:
            raise Exception('Invalid alphabet', 60)

        # return list of unique symbols
        alphabet = list(set([''] + re.findall(self.sym_re, alphabet, flags=re.UNICODE | re.DOTALL)))
        if self.white_delimiter:
            # remove apostrophes
            for i in range(len(alphabet)):
                if len(alphabet[i]) > 2:
                    alphabet[i] = alphabet[i][1:-1]
        return alphabet

        # change escaped '' to one symbol - '
        # return ['']+[c if c != "''" else "'" for c in re.findall(self.sym_re, alphabet, flags=re.UNICODE | re.DOTALL)]

    def __parse_transitions(self, rules):
        rules = rules.strip()
        rules_re = r"(?:" \
                   r"{id}\s*" \
                   r"{sym}\s*" \
                   r"->\s*" \
                   r"{id}" \
                   r"(?:{delim}{id}\s*{sym}\s*->\s*{id})*" \
                   r"){{0,1}}".format(delim=self.delim_re, id=self.id_re, sym=self.sym_re)

        if re.fullmatch(rules_re, rules, flags=re.UNICODE | re.DOTALL) is None:
            raise Exception('Invalid set of rules', 60)

        rule_re = r"({id})\s*" \
                  r"{sym}\s*" \
                  r"->\s*" \
                  r"({id})".format(id=self.id_re, sym=self.sym_re)

        # list of unique rules
        rules = list(set(re.findall(rule_re, rules, flags=re.UNICODE | re.DOTALL)))
        if self.white_delimiter:
            # remove apostrophes
            rules = [list(r) for r in rules]
            for i in range(len(rules)):
                if len(rules[i][1]) > 2:
                    rules[i][1] = rules[i][1][1:-1]

        return self.__validate_rules(rules)

    def __validate_rules(self, rules):
        for r in rules:
            if (r[0] not in self.states) or (r[1] not in self.alphabet) or (r[2] not in self.states):
                raise ValueError("Unknown state or symbol in rule "
                                 "\"{0} '{1}' -> {2}\"".format(r[0], r[1], r[2]), 61)
        return rules

    def isvalid_fsa(self):
        """Validate parsed FSA. FSA must be deterministic finite automaton (DFA) without unreachable states and with
        at most one non-terminating state

        Returns:
            True and None if FSA is valid, otherwise False and error message

        """

        # check if FSA is deterministic finite automaton (DFA)
        if self.states == [] or self.alphabet == [] or self.transitions == [] or self.init_state is None:
            return False, 'FSA has empty part, probably missing transition part'

        if '' in self.__get_sym_by_current_state():
            return False, 'FSA with ε-moves'
        self.alphabet.remove('')  # FSA without ε-moves, so remove ε form alphabet

        for s in self.states:
            if len(self.__get_sym_by_current_state(s)) != len(set(self.__get_sym_by_current_state(s))):
                return False, 'State "{0}" is not deterministic'.format(s)

        # check if DFA is complete DFA
        alphabet_size = len(self.alphabet)
        for s in self.states:
            if len(self.__get_sym_by_current_state(s)) != alphabet_size:
                return False, 'State "{0}" is not complete'.format(s)

        # check if complete DFA has some unreachable states
        reachable_states = [self.init_state]
        expandable_states = [self.init_state]
        while expandable_states:
            state = expandable_states.pop()
            for s in self.__get_states_by_current_state(state):
                if s not in reachable_states:
                    # s is unseen state
                    reachable_states.append(s)
                    expandable_states.append(s)
        # find unreachable states
        if set(self.states).difference(reachable_states):
            return False, 'FSA with unreachable states - ' + ', '.join(set(self.states).difference(reachable_states))

        # check if complete DFA has some non-terminating states
        terminating_states = self.final_states[:]  # make new copy of final states
        expandable_states = self.final_states[:]
        while expandable_states:
            state = expandable_states.pop()
            for s in self.__get_states_by_next_state(state):
                if s not in terminating_states:
                    # s is unseen state
                    terminating_states.append(s)
                    expandable_states.append(s)
        # find non-terminating states
        non_terminating_states = set(self.states).difference(terminating_states)
        if len(non_terminating_states) == 1:
            # non-terminating trap-state(qFalse) found
            self.non_terminating_state = non_terminating_states.pop()
        elif len(non_terminating_states) > 1:
            return False, 'FSA with more then one non-terminating states - ' + \
                          ', '.join(non_terminating_states)

        # FSA is valid
        return True, None

    def __get_sym_by_current_state(self, state=None):
        if state:
            return [c[1] for c in self.transitions if c[0] == state]

        return [c[1] for c in self.transitions]

    def __get_states_by_current_state(self, state=None):
        if state:
            return [s[2] for s in self.transitions if s[0] == state]

        return [s[2] for s in self.transitions]

    def __get_states_by_next_state(self, next_state=None):
        if next_state:
            return [s[0] for s in self.transitions if s[2] == next_state]

        return [s[0] for s in self.transitions]

    def __get_states_by_next_states(self, next_states, rules=None):
        if rules is None:
            rules = self.transitions

        resulting_states = set()
        for r in rules:
            if r[2] in next_states:
                resulting_states.add(r[0])

        return resulting_states

    def __get_rules_by_current_states_and_sym(self, states, sym):
        resulting_rules = []
        for s in states:
            for r in self.transitions:
                if r[0] == s and r[1] == sym:
                    resulting_rules.append(r)

        return resulting_rules

    def __get_rule_by_current_states_and_sym(self, state, sym):
        # return first found rule or None otherwise
        for r in self.transitions:
            if r[0] == state and r[1] == sym:
                return r

        return None

    def minimization(self):
        """Minimize valid FSA saved in this object. FSA is changed in place."""

        if self.final_states:
            states_groups = [set(self.final_states), set(self.states).difference(self.final_states)]
        else:
            # list of final states could be empty then FSA has only one state => is already minimal
            return

        # start with minimization
        splitting = True  # flag
        while splitting:
            splitting = False
            new_states_groups = states_groups[:]
            for g in states_groups:
                for c in self.alphabet:
                    # find rules with state in sets and symbol c, store next state to var
                    rules = self.__get_rules_by_current_states_and_sym(g, c)
                    next_states = set([s[2] for s in rules])

                    # splitting logic
                    for gg in states_groups:
                        common_elements = next_states & gg
                        if common_elements == next_states:
                            # all elements from next_states are same as gg
                            break
                        elif common_elements:
                            # some states are from other group
                            if g in new_states_groups:
                                new_states_groups.remove(g)
                            # split g to two groups
                            new_states_groups.append(g - self.__get_states_by_next_states(common_elements, rules))
                            new_states_groups.append(self.__get_states_by_next_states(common_elements, rules))
                            splitting = True
                            break

            states_groups = new_states_groups[:]

        # update FSA
        self.__update_states(states_groups)
        self.__update_rules()
        self.__update_init_state()
        self.__update_final_states()
        self.normalize_parts()

    def __update_states(self, states_groups):
        self.states.clear()
        states = [list(s) for s in states_groups]
        self.states = states

    def __update_rules(self):
        for s in self.states:
            for c in self.alphabet:
                new_next_state = set()
                for old_s in s:
                    r = self.__get_rule_by_current_states_and_sym(old_s, c)
                    new_next_state.add(r[2])
                    self.transitions.remove(r)
                self.transitions.append((s, c, self.__find_state(new_next_state)))

    def __update_init_state(self):
        for s in self.states:
            if self.init_state in s:
                self.init_state = s
                break  # initial state is just one

    def __update_final_states(self):
        finals = set(self.final_states)
        self.final_states.clear()
        for s in self.states:
            if finals.issubset(set(s)):
                self.final_states.append(s)

    def __find_state(self, next_state_set):
        for s in self.states:
            if next_state_set.issubset(set(s)):
                return s

    def __find_next_state(self, current_state, sym):
        for r in self.transitions:
            if r[0] == current_state and r[1] == sym:
                return r[2]
        return None

    def normalize_parts(self):
        """Convert states and rules which are composed, to single string form."""
        # normalize states
        for i in range(len(self.states)):
            s = sorted(self.states[i])
            self.states[i] = '_'.join(s)

        # normalize rules
        for i in range(len(self.transitions)):
            r = self.transitions[i]
            r[0].sort()  # current state
            r[2].sort()  # next state
            self.transitions[i] = ('_'.join(r[0]), r[1], '_'.join(r[2]))

        # normalize initial state
        self.init_state.sort()
        self.init_state = '_'.join(self.init_state)

        # normalize final states
        for i in range(len(self.final_states)):
            s = sorted(self.final_states[i])
            self.final_states[i] = '_'.join(s)

    def analyze_str(self, string):
        """Analyzed if FSA could read string"""
        current_state = self.init_state
        for c in string:
            if c == "'":
                c = "''"
            if c not in self.alphabet:
                return -1
            else:
                current_state = self.__find_next_state(current_state, c)
        # whole string is read

        if current_state in self.final_states:
            # string analysis ends in final state
            return 1

        return 0

    def write(self, file=sys.stdout):
        """Convert FSA object to string in proper form and write it to file.

        Args:
            file: open file object.

        """
        file.write('(\n')

        # write states
        self.states.sort()      # ascending order
        file.write('{')
        file.write(', '.join(self.states))
        file.write('},\n')
        # write alphabet
        self.alphabet.sort()    # ascending order
        file.write("{'")
        file.write("', '".join(self.alphabet))
        file.write("'},\n")
        # write rules
        self.transitions.sort(key=lambda rule: ''.join([rule[0], rule[1]]))
        file.write('{\n')
        for r in self.transitions:
            rule_str = r[0] + ' ' + "'" + r[1] + "'" + ' -> ' + r[2]
            if r is self.transitions[-1]:
                rule_str += '\n'
            else:
                rule_str += ',\n'
            file.write(rule_str)
        file.write('},\n')
        # write initial state
        file.write(self.init_state)
        file.write(',\n')
        # write final states
        self.final_states.sort()
        file.write('{')
        file.write(', '.join(self.final_states))
        file.write('}\n')

        file.write(')')
