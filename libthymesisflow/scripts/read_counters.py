import sys

FIFO ="/tmp/thymesispcount"
fd=open(FIFO,"r")

def read_fifo():
    while True:
        line = fd.readline()
        if line !="":
            sys.stdout.write(line)

if __name__ == '__main__':
    read_fifo()
