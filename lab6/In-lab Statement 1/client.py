import socket
import sys

def client(clientID):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 10000)
    sock.connect(server_address)

    try:
        msg = f"Hello I am client and My id is {clientID}"
        sock.sendall(msg.encode())
        reply = sock.recv(256)
        print("Received from the server:", reply.decode())
    finally:
        sock.close()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        clientID = sys.argv[1]
        if clientID.isdigit() and 0 <= int(clientID) <= 9:
            client(clientID)
        else:
            print("Client ID has be a single digit from 0 to 9")
    else:
        print("Provide client ID as a command line argument")
