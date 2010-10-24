import sys
import tempfile 
print sys.argv[1:]


t = tempfile.NamedTemporaryFile(mode='w', suffix='.txt', prefix='tmp', delete=False)
n = t.name

print t
print n


MMTYPES = ['0014', '6020', '6038', '6048', 'A000', 'A004', 'A008', 'A00C', 'A01C', 'A020', 'A02C', 'A030', 'A034', 'A038', 'A040', 'A048', 'A04C', 'A050', 'A054', 'A058', 'A068 ', 'A06C', 'A070', '0000', '0004', '0007', '0019', '001D']
frames_to_send = []
message = []
for arg in sys.argv[1:]:
    if arg.upper() in MMTYPES:
	if len(message) != 0:
	    frames_to_send.append(message)
	message = []
	message.append(arg)
    else:
	message.append(arg)

frames_to_send.append(message)
	
print frames_to_send




for mess in frames_to_send:
    for a in mess[1:]:
	t.write(a)

