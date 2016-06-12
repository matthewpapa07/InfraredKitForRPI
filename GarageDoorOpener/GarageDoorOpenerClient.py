import socket

HOST = 'musicbox'    # The remote host
PORT = 1337              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall('Hello, world')
s.close()