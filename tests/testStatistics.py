import shlex,subprocess,pexpect,getpass,sys,shutil,datetime,time, paramiko

ipAddr1 = '1.1.1.1'
macAddr1 = '00:0C:C3:44:0C:F3'
ipAddr3 = '1.1.1.3'
macAddr3 = '00:19:CB:FD:68:1D'
bandH = '90M'
bandM = '30M'
bandL = '5M'
duration = 20
faifapy = '/home/kinto/source/faifapy/faifa.py'
iperfCommand = 'iperf -c %s -t %s -u -b %s'


ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(ipAddr3, username='mobimesh', password='mobimesh')
stdin, stdout, stderr = ssh.exec_command('iperf -s -u')



#faifapyTest = 'sudo ' + faifapy + ' a070 00:19:CB:FD:68:1D 0 a030 2 fc 6048 a01c'
faifapyTest = 'sudo ' + faifapy + ' a030 2 fc a070 00:19:CB:FD:68:1D 0 6048'
faifapyReset = 'sudo ' + faifapy + ' a01c'

iperfTrain = iperfCommand % (ipAddr3, 10, bandH)

iperfA = iperfCommand % (ipAddr1, duration, bandL)
iperfB = iperfCommand % (ipAddr3, duration, bandM)
iperfC = iperfCommand % (ipAddr3, duration, bandH)

output = open("/home/kinto/source/faifapy/tests/testStatistics.txt","w")




#pIperfA1 = subprocess.Popen(shlex.split(iperfA), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
subprocess.call(shlex.split(faifapyReset))
subprocess.call(shlex.split(iperfTrain))
subprocess.call(shlex.split(faifapyTest))
output.write('\n')
output.write('----------------AFTER TRAINING--------------')
output.write('\n')
shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output3.txt")

in2 = open("/home/kinto/source/faifapy/output3.txt","r")

for line in in2:
    if "Modulation for carrier" not in line:
        output.write(line)
        
        in2.close()
output.write('\n')
output.write('----------------AFTER TRAINING--------------')
output.write('\n')
subprocess.call(shlex.split(iperfB))
subprocess.call(shlex.split(faifapyTest))



shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output1.txt")
in1 = open("/home/kinto/source/faifapy/output1.txt","r")

for line in in1:
    if "Modulation for carrier" not in line:
        output.write(line)

in1.close()

subprocess.call(shlex.split(faifapyReset))
subprocess.call(shlex.split(iperfTrain))
subprocess.call(shlex.split(faifapyTest))

output.write('\n')
output.write('----------------AFTER TRAINING--------------')
output.write('\n')
shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output3.txt")

in2 = open("/home/kinto/source/faifapy/output3.txt","r")

for line in in2:
    if "Modulation for carrier" not in line:
        output.write(line)
        
        in2.close()
output.write('\n')
output.write('----------------AFTER TRAINING--------------')
output.write('\n')

pIperfB1 = subprocess.Popen(shlex.split(iperfB), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pIperfB2 = subprocess.Popen(shlex.split(iperfB), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
outB1 = pIperfB1.stdout.read()
print outB1
outB2 = pIperfB2.stdout.read()
print outB2
pIperfB1.wait()
pIperfB2.wait()

subprocess.call(shlex.split(faifapyTest))

shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output3.txt")

in2 = open("/home/kinto/source/faifapy/output3.txt","r")

for line in in2:
    if "Modulation for carrier" not in line:
        output.write(line)

in2.close()
output.close()

stdin, stdout, stderr = ssh.exec_command('killall iperf')
ssh.close()


