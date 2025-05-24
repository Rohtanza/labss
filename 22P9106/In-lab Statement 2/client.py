import socket
import sys

def InvertWordsWithoutVowels(s):
    vowels = "aeiouAEIOU"
    words = s.split()
    invertedWords = []

    for word in words:
        if not any(char in vowels for char in word):
            invertedWords.append(word[::-1])
        else:
            invertedWords.append(word)

    return ' '.join(invertedWords)

def client(msg):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 10000)
    sock.connect(server_address)

    try:
        sock.sendall(msg.encode())
        reply = sock.recv(1024)
        modMsg = reply.decode()
        print("Received from the server:", modMsg)

        finalMsg = InvertWordsWithoutVowels(modMsg)
        print("The final message is :", finalMsg)
    finally:
        sock.close()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        msg = ' '.join(sys.argv[1:])
        client(msg)
    else:
        print("Give a message as a command line argument")
