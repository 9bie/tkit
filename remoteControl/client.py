import socket
import struct
import ctypes

def main():
    s = socket.socket()
    s.bind(("0.0.0.0",12346))
    s.listen(20000)
    conn,addr = s.accept()
    while True:
        data = conn.recv(10240)
        

if __name__ == "__main__":
    pass    