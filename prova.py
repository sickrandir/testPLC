output = open("/home/kinto/trunk/test/output.txt","w")
for line in open("/home/kinto/trunk/test/out_stream.txt"):
    if "Number" in line:
        output.write(line)
output.close()
