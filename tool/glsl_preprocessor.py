#!/usr/bin/env python3

import sys

already_included = []
path = 0


def get_file_contents(filename):
    data = None
    with open(filename) as inputfile:
        data = inputfile.readlines()
    return data


def preprocess_file(filename):
    filename = "{}{}".format(path, filename)
    if filename in already_included:
        return ""
    data = get_file_contents(filename)
    output = "/* {} */\n\n".format(filename)
    for line in data:
        if line.startswith("#include"):
            output += preprocess_file("{}.glsl".format(line[len("#include"):].strip()))
        else:
            output += line
    output += "\n"
    return output


target = sys.argv[1]
output = sys.argv[2]
file = target
path_pos = target.rfind('/')
if path_pos == -1:
    path = ""
else:
    path = target[:(path_pos+1)]
    file = target[(path_pos+1):]

print('GLSL preprocessing, path: {} file: {}'.format(path, file))

with open(output, 'w') as ofile:
    ofile.write(preprocess_file(file))

