import sys
import serial
import time
import os
import argparse

'''
x = [0x30, 0x31, 0x32]
y = str(bytearray(x))
#bytearray(b'012')
'''
'''
x = '\x30\x31\x32'
y = [ord(c) for c in x]
#[48, 49, 50]
'''

'''
import binascii
x = [0x30, 0x31, 0x32]
y = bytearray(x)
z = binascii.b2a_hex(y)
#b'303132'
'''

'''
x = '303132'
y = bytearray.fromhex(x)
z = list(y)
#[48, 49, 50]
'''
'''
x = '303132'
y = bytearray.fromhex(x)
z = str(y)
#bytearray(b'012')
'''




def download(url,port,baud):
    
#url = "rtthread.rbl"
    readLen = 4096
    templen = 0
    #print(url)
    #print(port)
    #print(baud)
    fsize = os.path.getsize(url)
    #print(fsize)
    b = "%08X" % fsize
    listSize = list(bytearray.fromhex(b))
    ser = serial.Serial(port,baud,timeout = 1)
    print("Open Serial port: %s"%port)
    print("Get input file name: %s"%url)
    print("Get input file size: %d"%fsize)
    time.sleep(1)
    ser.write(listSize)
    count = 1
    print("Reading input file %s"%url)
    print("Writing flash (%d bytes):\n"%(fsize))
    print("Writing | ",end='')
    recv = ser.read(4)
    while recv != b'OK\r\n':
        if(len(recv) > 0):
            print(recv)
        recv = ser.read(4)
        time.sleep(0.01)
    f = open(url,'rb')
    time.sleep(0.1)
    try:
        while True:
            data = f.read(readLen)
            ser.write(data)
            #print(data)
            #time.sleep(0.01)
            templen += len(data)
            time.sleep(0.01)
            curTime = time.time()
            #print("-------%d-------\r\n"%templen)
            if(templen %4096 == 0 or templen == fsize):
                recv = ser.read(4)
                while recv != b'OK\r\n':
                    print(templen)
                    print(recv)
                    recv = ""
                    recv = ser.read(4)
                    time.sleep(0.1)
                if(((templen/fsize)*100) >= (2*count)):
                    count += 1
                    #print("%d-%d-%d"%((templen/fsize)*100,20*count, count))
                    print('#',end='')
                
                sys.stdout.flush()
            if(templen == fsize):
                f.close()
    finally:
        f.close()
        print(" | 100%% %.2fs\n"%(time.time()-curTime))
        print("Wrote %d bytes to flash!")
        print("Close input file %s\n"%url)
        print("Loading data done.  Thank you")
        exit()
'''
fsize = os.path.getsize(url)
print(fsize)
b = "%08X" % fsize
listSize = list(bytearray.fromhex(b))
ser = serial.Serial('COM37',921600,timeout = 1)
time.sleep(1)
ser.write(listSize)
recv = ser.read(4)
while recv != b'OK\r\n':
    if(len(recv) > 0):
        print(recv)
    recv = ser.read(4)
    time.sleep(0.01)
    
f = open(url,'rb')
time.sleep(0.1)
try:
    while True:
        data = f.read(readLen)
        ser.write(data)
        #print(data)
        #time.sleep(0.01)
        templen += len(data)
        print("-------%d-------\r\n"%templen)
        if(templen %4096 == 0 or templen == fsize):
            recv = ser.read(4)
            while recv != b'OK\r\n':
                print(templen)
                print(recv)
                recv = ""
                recv = ser.read(4)
            
                time.sleep(0.1)
            print("====%d======\r\n"%templen)
        if(templen == fsize):
            f.close()
finally:
    f.close()
    print("fclose")
'''
'''
try:
    f = open(url,'rb')
    print(fsize)
    while True:
        tempdata = f.read(64)
        if len(tempdata) == 64:
            print(tempdata)
        else:
            print(tempdata)
            break
finally:
    if f:
        f.close()
'''
'''
ser = serial.Serial('COM37',115200,timeout = 0.2)
while True:
    recv = ser.read(10)
    if(len(recv) > 0):
        print(recv)
    else:
        time.sleep(0.1)
        ser.write([0x30,0x31,0x32])
        ser.write("hello".encode())
        
'''
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port")
    parser.add_argument("--baud")
    parser.add_argument("--url")
    args = parser.parse_args()
    #print(type(time.time()))
    print("Board programming configuration and parameters:")
    print("         Using Port                    : %s"%args.port)
    print("         Using Programmer              : arduino")
    print("         Overriding Baud Rate          : %s"%args.baud)
    print("         Chip                          : BK7251\n")
    print("Begin loading:")
    #print (type(args.port))
    #print (type(args.baud))
    #print (type(args.url))
    #url = r"E:\BK7251\bk7221u_release\rtthread.rbl"
    #download(url,"COM10","921600")
    download(args.url,args.port,args.baud)

def _main():
    main()
    '''try:
        main()
    except FatalError as e:
        print('\nA fatal error occurred: %s' % e)
        sys.exit(2)'''
if __name__ == '__main__':
    _main()
