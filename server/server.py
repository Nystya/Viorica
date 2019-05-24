import socket
import threading
import recognize as r
import wave
import numpy as np
import scipy.io.wavfile

bind_ip = '0.0.0.0'
bind_port = 5090
record = False
stream_chunk = []

max_chk = 30

CHANNELS = 1
RATE = 10000
CHUNK = 64

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(1)
voice_command = wave.open('english.wav','w')
voice_command.setparams((1, 1, 10000, 0, 'NONE', 'not compressed'))

print('Listening on {}:{}'.format(bind_ip, bind_port))

def atoi(string):
    res = []
    partial = 0
    
    print(len(string))

    for i in xrange(len(string)):
        if i != '\n':
            partial = partial * 10 + (ord(string[i]) - ord('0'))
        elif partial != 0:
            res.append(partial)
            partial = 0
        else:
            continue

    return res

def update_wav():
    global stream_chunk

    numpy_data = np.array(stream_chunk, dtype='uint8')
    scipy.io.wavfile.write('english.wav', 10000, numpy_data)

    stream_chunk = []

def create_wav():
    global stream_chunk
    global voice_command

    values = np.array(stream_chunk, dtype='uint8')

    print("Got here")
    voice_command.writeframes(values)

    stream_chunk = []
    

def handle_client_connection(client_socket):
    global record
    global max_chk
    global stream_chunk
    data_str = client_socket.recv(CHUNK)

    data = map(ord, data_str)

    for el in data:
        stream_chunk.append(el)
        print(el)

    max_chk -= 1

    if (max_chk == 0):
        max_chk = 30
        update_wav()
        r.recognize_file()

    client_socket.send('f\n')
    client_socket.close()

while True:
    client_sock, address = server.accept()
    
    client_handler = threading.Thread(target=handle_client_connection, \
                     args=(client_sock,))

    client_handler.start()


