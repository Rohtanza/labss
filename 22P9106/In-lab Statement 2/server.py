import socket

def InvertWordsWithVowels(s):
    vowels = "aeiouAEIOU"
    words = s.split()
    invertedWords = []

    for word in words:
        if any(char in vowels for char in word):
            invertedWords.append(word[::-1])
        else:
            invertedWords.append(word)

    return ' '.join(invertedWords)

def server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 10000)
    sock.bind(server_address)
    sock.listen(1)

    print("Server is listening on the port no 10000")

    while True:
        connection, client_address = sock.accept()
        try:
            print("Connection from Client:", client_address)
            msg = connection.recv(1024)
            if msg:
                message = msg.decode()
                print("Received message:", message)

                modMsg = InvertWordsWithVowels(message)
                print("Modified message:", modMsg)

                connection.sendall(modMsg.encode())
            else:
                print("No data from client:", client_address)
        finally:
            connection.close()

if __name__ == "__main__":
    server()
