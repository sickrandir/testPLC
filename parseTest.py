import re, decimal

output = open("/home/kinto/source/faifapy/parseTest.txt","w")
infile = open("/home/kinto/source/faifapy/output3.txt","r")

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
    
    
    
def parseA048_A049(lines):
    output.writelines(lines)



matchDict= {
    'Number of modulation:.': [parseA070_A071, -9 , -1],
    'Turbo Bit Errors failed.': [parseA030_A031, -14 , 0],
    'Average data rate from DA to STA:.': [parseA048_A049, -2 , 0] 
    }
         
                
for idx, line in enumerate(lines_list):
    for k, v in matchDict.items():
        if re.match(k,line):
            #print v[0]
            #parseA070_A071(lines_list[idx+v[1]:idx+v[2]])
            v[0](lines_list[idx+v[1]:idx+v[2]+1])
                
infile.close()
output.close()
                  
                        