#!/usr/bin/env python

import os
import ctypes
import subprocess
import threading
import tempfile
import sys
from time import sleep
import shutil
import re
import decimal

MMTYPES = ['0014', '6020', '6038', '6048', 'A000', 'A004', 'A008', 'A00C', 'A01C', 'A020', 'A02C', 'A030', 'A034', 'A038', 'A040', 'A048', 'A04C', 'A050', 'A054', 'A058', 'A068 ', 'A06C', 'A070', '0000', '0004', '0007', '0019', '001D']
libc_path = '/lib/libc.so.6'
libfaifa_path = '/usr/lib/libfaifa.so.0'
#libfaifa_path = '/home/kinto/trunk/libfaifa.so.0'

def main():
    
    def receive_loop(faifa):
        libfaifa.faifa_loop(faifa, libfaifa.do_receive_frame, faifa)   
        
    #Batch Mode: load faifa library and prepare the input file with the frames to send and their arguments
    def batch_setup():
        message = []
        for arg in sys.argv[1:]:
            if arg.upper() in MMTYPES:
                if len(message) != 0:
                    frames_to_send.append(message)
                message = []
                message.append(arg)
            else:
                if len(message) != 0:
                    message.append(arg)
    
        if len(message) != 0:
            frames_to_send.append(message)
        
        if len(frames_to_send) == 0:
            sys.exit("\nWrong arguments don't correspend to frames to send with faifa\n")

        temp = tempfile.NamedTemporaryFile(mode='w', suffix='.txt', prefix='tmp', delete=True)
        #global temp_file
        temp_file = temp.name
        for mess in frames_to_send:
            for a in mess[1:]:
                temp.write(a)
        
        #and can then load the library as follows:
        
        global libfaifa
        libfaifa = ctypes.cdll.LoadLibrary(libfaifa_path)
        global libc
        libc =  ctypes.cdll.LoadLibrary(libc_path)
        out_stream_p = ctypes.c_void_p.in_dll(libfaifa,'out_stream')
        in_stream_p = ctypes.c_void_p.in_dll(libfaifa,'in_stream')
        err_stream_p = ctypes.c_void_p.in_dll(libfaifa,'err_stream')
    
        in_stream_file = libc.fopen(temp_file,'r')
        global out_stream_file
        out_stream_file = libc.fopen('/home/kinto/source/faifapy/out_stream.txt','w')
        stderr_p = ctypes.c_void_p.in_dll(libfaifa,'stderr')
        #definition of input, output, error files
        #defining an input file different from stdin is mandatory to automate certain messsage exchanges
        out_stream_p.value = out_stream_file
        in_stream_p.value = in_stream_file
        err_stream_p.value = stderr_p.value
    
    
    #Interactive Mode: load faifa library using STDIN, STDOU and STDERR for communication
    
    def interactive_setup():
        global libfaifa
        libfaifa = ctypes.cdll.LoadLibrary(libfaifa_path)
        global libc
        libc =  ctypes.cdll.LoadLibrary(libc_path)
        out_stream_p = ctypes.c_void_p.in_dll(libfaifa,'out_stream')
        in_stream_p = ctypes.c_void_p.in_dll(libfaifa,'in_stream')
        err_stream_p = ctypes.c_void_p.in_dll(libfaifa,'err_stream')
    
        #definition of input, output, error files
        #defining an input file different from stdin is mandatory to automate certain messsage exchanges

        stdout_p = ctypes.c_void_p.in_dll(libfaifa,'stdout')
        stdin_p = ctypes.c_void_p.in_dll(libfaifa,'stdin')
        stderr_p = ctypes.c_void_p.in_dll(libfaifa,'stderr')
        out_stream_p.value = stdout_p.value
        in_stream_p.value = stdin_p.value
        err_stream_p.value = stderr_p.value

    #Calls libfaifa funtions to setup and start the program and spawns a thread for the receive loop
    
    def faifa_startup():
        global faifa 
        faifa = ctypes.c_void_p(libfaifa.faifa_init())
        state = libfaifa.faifa_open(faifa, 'eth0') 
        #create a daemon thread for the receive loop, so when the foreground sending of messages is over, the program doesn't wait for the receive loop thread to terminate.
        receive_loop_t = threading.Thread(group=None, target=receive_loop, name='receive_loop', args=(faifa,), kwargs={})
        receive_loop_t.daemon = True
        receive_loop_t.start()
    
    #def calcRateBound(lines):
        #bitTX = 0
        #bitPerSymbol = {
            #"NO": 0,
            #"Unknown/unused": 0,
            #"BPSK": 1,
            #"QPSK": 2,
            #"QAM-8": 3,
            #"QAM-16": 4,
            #"QAM-64": 6,
            #"QAM-256": 8,
            #"QAM-1024": 10,
            #}
        #for line in lines:
            #words = line.split()
            #bits = bitPerSymbol[words[4]]
            #print words[4] + " -> " + str(bits)
            #bitTX = bitTX + bits*int(words[6])
    
        ##print str(bitTX) + " bit totali trasmessi nel tempo di simbolo"
        #bitPerSecond = round(bitTX/0.0000465)
        ##print str(bitPerSecond) + " bit/s"
        ##print str(bitPerSecond/1000000) + " Mbit/s"
    
    
      
    
    
    
    #Sends frame in batch mode
    
    def batch():
        for mess in frames_to_send:
            s = mess[0]
            mmtype = int(s,16)
            libfaifa.do_frame(faifa, mmtype, None, None, None)
            sleep(3)



    #Sends frame in interactive mode    
    
    def interactive():
        while(True):
            s = raw_input("Choose the MMTYPE to send:")
            mmtype = int(s,16)
            libfaifa.do_frame(faifa, mmtype, None, None, None)
            sleep(3)
            s = raw_input("Press enter to send another frame...")
    
   
    
    
    #Only root can properly use faifa library
    
    if not os.geteuid()==0:
        sys.exit("\nOnly root can run this script\n")
   
    frames_to_send = []
    if len(sys.argv) == 1:
        interactive_setup()
        faifa_startup()
        interactive()
    else:
        batch_setup()
        faifa_startup()
        batch()
    
    close_res = libc.fclose(out_stream_file)
    #print "close: " + str(close_res)    
    #libfaifa.faifa_close(faifa)
    #libfaifa.faifa_free(faifa)
    
    shutil.copy("/home/kinto/source/faifapy/out_stream.txt", "/home/kinto/source/faifapy/output_bak.txt")
    
    output = open("/home/kinto/source/faifapy/output.txt","w")
    infile = open("/home/kinto/source/faifapy/output_bak.txt","r")

    lines_list = infile.readlines()

    def parseA070_A071(lines):
        bitTX = 0
        bit_per_symbol = {
            "NO": 0,
            "Unknown/unused": 0,
            "BPSK": 1,
            "QPSK": 2,
            "QAM-8": 3,
            "QAM-16": 4,
            "QAM-64": 6,
            "QAM-256": 8,
            "QAM-1024": 10,
            }
        for line in lines:
            words = line.split()
            bits = bit_per_symbol[words[4]]
            #print words[4] + " -> " + str(bits)
            bitTX = bitTX + bits*int(words[6])
        
        #print str(bitTX) + " bit totali trasmessi nel tempo di simbolo"
        bit_per_second = round(bitTX/0.0000465)
        #print str(bit_per_second) + " bit/s"
        output.writelines(str(bit_per_second/1000000) + " Mbit/s\n")
        
    def parseA030_A031(lines):
        words = lines[3].split()
        tx_mpdu_OK = int(words[2])
        words = lines[4].split()
        tx_mpdu_collided = int(words[2])
        words = lines[5].split()
        tx_mpdu_failed = int(words[2])
        words = lines[6].split()
        tx_pb_OK = int(words[3])
        words = lines[7].split()
        tx_pb_failed = int(words[3])
        
        tx_mpdu_total = tx_mpdu_OK + tx_mpdu_collided + tx_mpdu_failed
        decimal.getcontext().prec = 5
        tx_mpdu_collision_perc = decimal.Decimal(tx_mpdu_collided) / decimal.Decimal(tx_mpdu_total) * 100
        tx_mpdu_failure_perc = decimal.Decimal(tx_mpdu_failed) / decimal.Decimal(tx_mpdu_total) * 100
        
        tx_pb_total = tx_pb_OK + tx_pb_failed
        tx_pb_failure_perc = decimal.Decimal(tx_pb_failed) / decimal.Decimal(tx_pb_total) * 100
        
        words = lines[9].split()
        rx_mpdu_OK = int(words[2])
        words = lines[10].split()
        rx_mpdu_failed = int(words[2])
        words = lines[11].split()
        rx_pb_OK = int(words[3])
        words = lines[12].split()
        rx_pb_failed = int(words[3])
        words = lines[13].split()
        rx_tbe_OK = int(words[4])
        words = lines[14].split()
        rx_tbe_failed = int(words[4])
        
        rx_mpdu_total = rx_mpdu_OK + rx_mpdu_failed
        rx_mpdu_failure_perc = decimal.Decimal(rx_mpdu_failed) / decimal.Decimal(rx_mpdu_total) * 100
        rx_pb_total = rx_pb_OK + rx_pb_failed
        rx_pb_failure_perc = decimal.Decimal(rx_pb_failed) / decimal.Decimal(rx_pb_total) * 100
        
        rx_tbe_total = rx_tbe_OK + rx_tbe_failed
        rx_tbe_failure_perc = decimal.Decimal(rx_tbe_failed) / decimal.Decimal(rx_tbe_total) * 100
        
        lines.insert(6, "TX MPDU collision percentage: " + str(tx_mpdu_collision_perc)+" %\n")
        lines.insert(7, "TX MPDU failure percentage: " + str(tx_mpdu_failure_perc)+" %\n")
        lines.insert(10, "TX PB failure percentage: " + str(tx_pb_failure_perc)+" %\n")
        lines.insert(14, "RX MPDU failure percentage: " + str(rx_mpdu_failure_perc)+" %\n")
        lines.insert(17, "RX PB failure percentage: " + str(rx_pb_failure_perc)+" %\n")
        lines.append("RX TBE failure percentage: " + str(rx_tbe_failure_perc)+" %\n")
        output.writelines(lines)
        
        
        
    def parse6048_6049(lines):
       output.writelines(lines)



    matchDict= {
        'Number of modulation:.': [parseA070_A071, -9 , -1],
        'Turbo Bit Errors failed.': [parseA030_A031, -14 , 0],
        'Average data rate from DA to STA:.': [parse6048_6049, -2 , 0] 
        }
            
                    
    for idx, line in enumerate(lines_list):
        for k, v in matchDict.items():
            if re.match(k,line):
                v[0](lines_list[idx+v[1]:idx+v[2]+1])
                    
    infile.close()
    output.close()
                  
   


if __name__ == '__main__':
    main()   


#mmtype = c_ushort(int(s,16))
#a074='000cc3440cf408002745cf3b88e10074a000b05200000000000000000000000000000000000000040300060056010901000cc3440cf30200156dc330'
#packet = a074.decode('hex')
