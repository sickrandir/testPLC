import shlex,subprocess

ipAddr1 = '1.1.1.3'

bandH = '90M'
bandM = '40M'
bandL = '5M'
duration = 20

iperfCommand = 'iperf -c %s -t %s -u -b %s'
iperfB = iperfCommand % (ipAddr1, duration, bandM)

pIperfB2 = subprocess.Popen(shlex.split(iperfB), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
pIperfB3 = subprocess.Popen(shlex.split(iperfB), stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

pIperfB2.wait()
pIperfB3.wait()