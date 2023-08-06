#!/usr/bin/env python3

import sys

for line in sys.stdin:
    try:
        line = line.strip()
        pattern = "_preprocessed.glsl:"
        preprocessed_pos = line.find(pattern)
        if preprocessed_pos != -1:
            filename_start_pos = line[:preprocessed_pos].rfind(" ")
            filename_end_pos = line[preprocessed_pos:].find(":")
            filename = line[filename_start_pos+1:preprocessed_pos+filename_end_pos]
            line_number = line[preprocessed_pos+filename_end_pos+1:]
            line_number_end = line_number.find(":")
            line_number = int(line_number[:line_number_end])
            with open(filename) as preprocessed_source:
                lines = preprocessed_source.readlines()
                prep_line = lines[line_number-1]
                src_map_pattern = "// from: "
                src_map_pos = prep_line.find(src_map_pattern)
                if src_map_pos != -1:
                    true_src = prep_line[src_map_pos+len(src_map_pattern):].strip()
                    print("{}{}{}".format(line[:filename_start_pos+1], true_src, line[preprocessed_pos+filename_end_pos+line_number_end+1:]))
                else:
                    print(line)
        else:
            print(line)
    except:
        print(line)
