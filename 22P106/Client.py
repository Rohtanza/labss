import socket

## client file


def start_client():
    # creating a ucp socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    server_address = ('localhost', 2000)

    while True:
        try:
            # getting input from the user
            client_message = input(
                "Enter Message (e.g., 00-0000-CI (Check in) or 00-0000-CO (Check Out)): ")

            # sending the message to the server
            sock.sendto(client_message.encode(), server_address)

            # reciveing the response from the server
            server_message, _ = sock.recvfrom(2000)
            print(f"The server Message: {server_message.decode()}")

        except Exception as e:
            print(f"A error has occurred: {e}")


if __name__ == "__main__":
    start_client()
