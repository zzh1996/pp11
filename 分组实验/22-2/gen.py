#!/usr/bin/env python3

import random
import sys

L = int(sys.argv[1])
N = int(sys.argv[2])

print(L)

for i in range(L):
    print(random.randrange(0, L), end=" ")
print()
for i in range(L):
    print(random.randrange(0, L), end=" ")
print()

print(N)

for i in range(N):
    print(random.randrange(0, L), end=" ")
print()