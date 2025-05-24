import socket

def server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 10000)
    sock.bind(server_address)
    sock.listen(1)

    print("Server is listening on port no 10000")

    while True:
        connection, client_address = sock.accept()
        try:
            print("Connection from Client:", client_address)
            msg = connection.recv(256)
            if msg:
                message = msg.decode()
                print("Received message:", message)

                try:
                    clientID = int(message.split()[-1])
                    if 0 <= clientID <= 9:
                        com_id = 9 - clientID
                        server_reply = f"Hello I am Server. Your complement id is {com_id}"
                        connection.sendall(server_reply.encode())
                    else:
                        connection.sendall("Client ID has be a single digit from 0 to 9".encode())
                except ValueError:
                    connection.sendall("Client ID format is invalid".encode())
            else:
                print("No data from the client:", client_address)
        finally:
            connection.close()

if __name__ == "__main__":
    server()
