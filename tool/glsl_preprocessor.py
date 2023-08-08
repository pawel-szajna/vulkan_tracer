#!/usr/bin/env python3

import sys

already_included = []
path = 0


def get_file_contents(filename, requestor, requestor_line=None):
    data = None
    try:
        with open(filename) as inputfile:
            data = inputfile.readlines()
    except FileNotFoundError:
        print("ERROR: {}: Included file \"{}\" not found".format(requestor, filename))
        if requestor_line:
            print("    {}".format(requestor_line))
        sys.exit(-1)
    return data


def preprocess_file(filename, requestor, requestor_line=None):
    if "builtin" in filename:
        return ""
    while filename.startswith("../"):
        filename = filename[3:]
    if not filename.startswith("/"):
        filename = "{}{}".format(path, filename)
    if filename in already_included:
        return ""
    already_included.append(filename)
    data = get_file_contents(filename, requestor, requestor_line)
    output = "/* {} */\n\n".format(filename)
    linectr = 1
    for line in data:
        if line.startswith("#include"):
            include_value = line[len("#include"):].strip()
            if include_value.startswith("generated"):
                include_value = "{}/{}".format(gen_dir, include_value[len("generated"):].strip())
            if include_value.startswith('"') or include_value.startswith("<"):
                include_value = include_value[1:]
            if include_value.endswith('"') or include_value.endswith(">"):
                include_value = include_value[:-1]
            if not include_value.endswith(".glsl"):
                include_value = include_value + ".glsl"
            output += preprocess_file(include_value, "{}:{}".format(filename, linectr), line)
        else:
            if not line.startswith("#pragma once"):
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
    ofile.write(preprocess_file(file, "(root)"))

