#lines = []
#lines.append("Number of carriers with NO modulation: 761 (65.000000 %)")
#lines.append("Number of carriers with BPSK modulation: 106 (9.000000 %)")
#lines.append("Number of carriers with QPSK modulation: 162 (14.000000 %)")
#lines.append("Number of carriers with QAM-8 modulation: 52 (4.000000 %)")
#lines.append("Number of carriers with QAM-16 modulation: 75 (6.000000 %)")
#lines.append("Number of carriers with QAM-64 modulation: 0 (0.000000 %)")
#lines.append("Number of carriers with QAM-256 modulation: 0 (0.000000 %)")
#lines.append("Number of carriers with QAM-1024 modulation: 0 (0.000000 %)")
#lines.append("Number of carriers with Unknown/unused modulation: 0 (0.000000 %)")

input = open("./temp.txt")
lines = input.readlines()

def calcRateBound(lines):
    bitTX = 0
    bitPerSymbol = {
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
	bits = bitPerSymbol[words[4]]
	print words[4] + " -> " + str(bits)
	bitTX = bitTX + bits*int(words[6])
	
    print str(bitTX) + " bit totali trasmessi nel tempo di simbolo"
    bitPerSecond = round(bitTX/0.0000465)
    print str(bitPerSecond) + " bit/s"
    print str(bitPerSecond/1000000) + " Mbit/s"
    



calcRateBound(lines)