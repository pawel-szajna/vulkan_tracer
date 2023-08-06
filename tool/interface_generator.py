#!/usr/bin/env python3

import datetime
import json
import sys


if len(sys.argv) < 4:
    print("usage: interface_generator.py source.json target_dir mode")
    sys.exit(1)

source = sys.argv[1]
target = sys.argv[2]
mode = sys.argv[3]

if mode != "cpp" and mode != "glsl":
    print("Invalid mode: {}, expected cpp or glsl".format(mode))
    sys.exit(1)

with open(source) as srcfile:
    data = json.load(srcfile)

extension = "hpp" if mode == "cpp" else "glsl"
ext_pos = source.rfind(".")
source = source[:ext_pos]
path_pos = source.rfind("/")
if path_pos != -1:
    source = source[path_pos+1:]
name = "{}/{}.{}".format(target, source, extension)


type_map = {
    "cpp": {
        "uint": "u32",
        "int": "i32",
        "float": "float",
        "vec4": "float",
        "size_t": "usize"
    },
    "glsl": {
        "uint": "uint",
        "int": "int",
        "float": "float",
        "vec4": "vec4",
        "size_t": "uint"
    }
}

alignment = {
    "uint": 4,
    "int": 4,
    "float": 4,
    "vec4": 16
}


def gen_type(t):
    return type_map[mode][t]


def gen_buffer(entry):
    out = ""
    if mode == "cpp":
        out += "struct {}\n{{\n".format(entry["name"])
    else:
        out += "layout(set = {}, binding = {}, std430)\n".format(entry["set"], entry["binding"])
        out += "buffer {}\n{{\n".format(entry["name"])
    for field in entry["fields"]:
        arr = ""
        align = ""
        count = 1
        if "size" in field:
            count = field["size"]
        if mode == "cpp" and field["type"] == "vec4":
            if type(count) == int:
                count *= 4
            else:
                count = "{} * 4".format(count)
        if type(count) == int and count > 1 or not type(count) == int:
            arr = "[{}]".format(count)
        if mode == "cpp":
            align = "alignas({}) ".format(alignment[field["type"]])
        out += "    {}{} {}{};\n".format(align, gen_type(field["type"]), field["name"], arr)
    if mode == "cpp":
        out += "};\n\n"
    else:
        out += "}}\n{};\n\n".format(entry["bufferName"])
    return out


def gen_constant(entry):
    out = ""
    if mode == "cpp":
        out += "constexpr "
    else:
        out += "const "
    out += "{} {}".format(gen_type(entry["type"]), entry["name"])
    if mode == "cpp":
        out += "{{{}}};\n".format(entry["value"])
    else:
        out += " = {};\n".format(entry["value"])
    return out


with open(name, 'w') as output:
    if mode == "cpp":
        output.write("#pragma once\n\n")
        if "depends" in data:
            for dependency in data["depends"]:
                output.write("#include \"{}.hpp\"\n".format(dependency))
            output.write("\n")
    output.write("// Auto-generated on {}\n".format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
    output.write("// To introduce changes, modify {}.json instead of this file\n\n".format(source))
    for entry in data["gen"]:
        if mode not in entry or entry[mode] is False:
            continue
        if entry["kind"] == "buffer":
            output.write(gen_buffer(entry))
        if entry["kind"] == "constant":
            output.write(gen_constant(entry))
    output.write("\n")
