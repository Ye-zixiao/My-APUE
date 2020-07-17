outfile=open("outfile","w")
for i in range(100):
    outfile.write(str(i)+"hello world\n")

outfile.close()
