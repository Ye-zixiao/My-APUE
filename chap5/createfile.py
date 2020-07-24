#!/usr/bin/python3
outfile=open("infile","w")
for i in range(3000000):
    outfile.write(str(i)+"hello world\n");
outfile.close()
