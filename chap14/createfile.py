#!/usr/bin/python3
infile=open("infile","w")
for i in range(1000000):
    infile.write(f"{i}: hello world\n")
infile.close()
