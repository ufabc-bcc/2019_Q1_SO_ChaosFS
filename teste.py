#!/usr/bin/env python
import sys

size = sys.argv[1]
dir  = sys.argv[2]

if dir[-1] != '/':
    dir += '/'


for x in range(int(size)):
    name = f"{bin(x)[2:]:>15}".replace(' ', '0')
    
    f = open(dir + name, 'w')
    f.writelines(f"aaa {x}")
    f.close()
