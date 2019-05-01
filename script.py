import sys

size = sys.argv[1]
for x in range(int(size)):
    f = open(f"a{x}", 'w')
    f.writelines(f"aaa {x}")
    f.close()
