import os
import sys
import re

if len(sys.argv) != 3:
    exit(-1)

input_file = open(sys.argv[1], 'r')
output_file = open(sys.argv[2], 'w')

output_file.write("#ifndef __AUTOCFG_H__\n")
output_file.write("#define __AUTOCFG_H__\n\n")

for line in input_file.readlines():
    matchObj = re.search( r'.*( *)?=( *)?[nN]', line, re.M|re.I)
    if matchObj:
        pass
    else:
        str = re.sub(r'( *)?=( *)?', " ", line)
        output_file.write("#define ")
        output_file.write(str)

output_file.write("\n\n")
output_file.write("#endif\n")

input_file.close()
output_file.close()