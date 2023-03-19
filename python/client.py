import socket

csocket = socket.socket(socket.AF_INET,  socket.SOCK_STREAM, socket.IPPROTO_TCP)
csocket.connect(('192.168.8.132', 4500))
while True:
    sendmsg = input()
    csocket.send(sendmsg.encode('utf-8'))
    recvmsg = csocket.recv(1024)
    print('Received {0}'.format(recvmsg.decode('utf-8')))
