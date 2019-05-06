#!/usr/bin/env python
import sys
import time
from tqdm import tqdm

size = sys.argv[1]
dir  = sys.argv[2]

if dir[-1] != '/':
    dir += '/'

size = int(size)
conteudo = "inicio" + 4084*"a" + "final\n"

pbar = tqdm(total=size, ncols=100)
for x in range(size):
    pbar.update(1)

    name = f"{bin(x)[2:]:>15}".replace(' ', '0')

    with open(dir + name, 'w') as f:
        f.writelines(conteudo)

pbar.close()