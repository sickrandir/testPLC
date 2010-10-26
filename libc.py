import ctypes 
from threading import *
import sys
#and can then load the library as follows:
lib = "/lib/libc.so.6"
libc = ctypes.cdll.LoadLibrary(lib)

#print libc.time(None)

#libc.printf("Hello, %s\n", "World!")

#dll.fprintf(out_stream_p, "%d", i)



def errcheck(res, func, args):
  if not res:
    raise IOError('Error opening file')
  return res

libc.fopen.errcheck = errcheck
#libc.fopen('notexist', 'r')
stdout_p = ctypes.c_void_p.in_dll(libc,'stdout')
#print libc.stdout


print libc.fprintf(stdout_p, "Hello, %s\n", "World!")
