import shlex,subprocess,pexpect,getpass,sys,shutil,datetime,time

ipAddr1 = '1.1.1.1'
macAddr1 = '00:0C:C3:44:0C:F3'
ipAddr3 = '1.1.1.3'
macAddr3 = '00:24:B2:57:20:32'
bandH = '90M'
bandM = '40M'
bandL = '5M'
duration = 10
faifapy = '/home/kinto/source/faifapy/faifa.py'
iperfCommand='iperf -c %s -t %s -u -b %s'

faifapyCommand='sudo ' + faifapy + ' a070 00:24:B2:57:20:32 0 a070 00:0C:C3:44:0C:F3 0 a038 a070 00:24:B2:57:20:32 0 a070 00:0C:C3:44:0C:F3 0 a038 a070 00:24:B2:57:20:32 0 a070 00:0C:C3:44:0C:F3 0 a038 a070 00:24:B2:57:20:32 0 a070 00:0C:C3:44:0C:F3 0 a038'

iperfA = iperfCommand % (ipAddr1, duration, bandL)
iperfB = iperfCommand % (ipAddr3, duration, bandL)
iperfC = iperfCommand % (ipAddr3, duration, bandH)

output = open("/home/kinto/source/faifapy/tests/testTonemap.txt","w")



pIperfA1 = subprocess.Popen(shlex.split(iperfA), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pIperfB1 = subprocess.Popen(shlex.split(iperfB), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pFaifa1 = subprocess.Popen(shlex.split(faifapyCommand), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

pFaifa1.wait()

shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output1.txt")
in1 = open("/home/kinto/source/faifapy/output1.txt","r")

for line in in1:
    if "Modulation for carrier" not in line:
        output.write(line)

in1.close()



pIperfA2 = subprocess.Popen(shlex.split(iperfA), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pIperfC1 = subprocess.Popen(shlex.split(iperfC), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pFaifa2 = subprocess.Popen(shlex.split(faifapyCommand), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

pFaifa2.wait()

shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output2.txt")

in2 = open("/home/kinto/source/faifapy/output2.txt","r")

for line in in2:
    if "Modulation for carrier" not in line:
        output.write(line)

in2.close()
output.close()
