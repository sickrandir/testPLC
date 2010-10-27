# -*- coding: utf-8 -*-
import shlex,subprocess,pexpect,getpass,sys,shutil,datetime,time,paramiko

remoteAddr = '192.168.1.218'
band1 = '5M'
band2 = '10M'
band3 = '15M'
duration = 7

now = datetime.datetime.now()
timenow = now.strftime('%Y-%m-%d_%H:%M')
dumpDir = '/home/kinto/Documents/dumpCaptures/'
dumpFile = 'dump-'+timenow+'.pcap'
dumpA03839 = 'dumpA03839-'+timenow+'.pcap'
dumpA07475 = 'dumpA07475-'+timenow+'.pcap'
videoOutput = 'videoNetInfo-'+timenow+'.ogv'

#Avvio il server iperf remoto
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(
paramiko.AutoAddPolicy())
ssh.connect(remoteAddr, username='kinto', password='db1018')
stdin, stdout, stderr = ssh.exec_command('iperf -s -u')
stdin, stdout, stderr = ssh.exec_command('pjsua-i686-pc-linux-gnu --null-audio --add-codec=pcma --play-file=/home/kinto/call001.wav --auto-play --auto-answer=200 ')



#xwininfo = 'xwininfo -name "Windows XP [Running] - Sun VirtualBox"'
#args = shlex.split(xwininfo)
#px = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
#px.wait()
#for line in px.stdout:
  #if "Window id:" in line:
    #winid = line[21:30]


#recordCommand = 'sudo -u kinto recordmydesktop --no-sound -windowid '+winid+' -o '+dumpDir+videoOutput
#args = shlex.split(recordCommand)
#pvideo = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

#dumpCommand='sudo dumpcap -i eth0 -a duration:60 -f "ether proto 0x88e1"'
dumpCommand='sudo dumpcap -i eth0 -a duration:60'
args = shlex.split(dumpCommand)
pdump = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
line = pdump.stdout.readline()
tmpCap = line[6:-1]

iperfCommand='iperf -c %s -t %s -d -u -b %s'
iperf1=iperfCommand % (remoteAddr, duration, band1)
iperf2=iperfCommand % (remoteAddr, duration, band2)
iperf3=iperfCommand % (remoteAddr, duration, band3)
time.sleep(1)
subprocess.call(shlex.split(iperf1))
time.sleep(3)
subprocess.call(shlex.split(iperf2))
time.sleep(3)
subprocess.call(shlex.split(iperf3))

#Chiusura server iperf remoto e connession SSH
stdin, stdout, stderr = ssh.exec_command('killall iperf')
ssh.close()



pdump.wait()
pvideo.terminate()
shutil.copy(tmpCap, dumpDir+dumpFile)

subprocess.call(shlex.split('chown kinto:kinto '+dumpDir+dumpFile))

subprocess.call(shlex.split('sudo -u kinto tshark -r '+dumpDir+dumpFile+' -R "eth[15-16]==38:a0 or eth[15-16]==39:a0" -w '+dumpDir+dumpA03839))
subprocess.call(shlex.split('sudo -u kinto tshark -r '+dumpDir+dumpFile+' -R "eth[15-16]==74:a0 or eth[15-16]==75:a0" -w '+dumpDir+dumpA07475))
