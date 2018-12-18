#!/usr/bin/env python3

import random
import sys

size = int(sys.argv[1])

print(size, size)

for i in range(size):
    for j in range(size):
        print(random.uniform(0, size), end=" ")
    print()
