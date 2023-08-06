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
    if not filename.startswith("/"):
        filename = "{}{}".format(path, filename)
    if filename in already_included:
        return ""
    data = get_file_contents(filename)
    output = "/* {} */\n\n".format(filename)
    linectr = 1
    for line in data:
        if line.startswith("#include"):
            include_value = line[len("#include"):].strip()
            if include_value.startswith("generated"):
                include_value = "{}/{}".format(gen_dir, include_value[len("generated"):].strip())
            output += preprocess_file("{}.glsl".format(include_value))
        else:
            output += "{}    // from: {}:{}\n".format(line.rstrip(), filename, linectr)
        linectr += 1
    output += "\n"
    return output


target = sys.argv[1]
output = sys.argv[2]
gen_dir = sys.argv[3]
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

