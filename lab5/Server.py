import socket


def start_server():
    # creating ucp socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # connecting the socket to the server address and port
    server_address = ('localhost', 2000)
    sock.bind(server_address)

    # creating a list by the name of the database, to store checked in students
    database = []
    error_count = 0

    print("Server started. Now listening for the msg:")

    while True:
        # reciving the msg from the client
        client_message, client_address = sock.recvfrom(2000)
        message = client_message.decode()
        print(f"The received message from {client_address}: {message}")

        # proccess the message we got
        if message.endswith("-CI"):
            roll_number = message[:-3]
            if roll_number in database:
                response = "You are present in the database."
            else:
                database.append(roll_number)
                response = f"Welcome Student {roll_number}"
        elif message.endswith("-CO"):
            roll_number = message[:-3]
            if roll_number in database:
                database.remove(roll_number)
                response = f"Bye Bye Student with the roll no: {roll_number}"
            else:
                response = "You are not present in the database, So you cant checkout as your didnt even check in."
                error_count += 1
        else:
            response = "Wrong message format."
            error_count += 1

        # sending the response back to the client
        sock.sendto(response.encode(), client_address)

        # printing the current database state
        print("Current Database Condition:", database)
        print("Error Count:", error_count)


if __name__ == "__main__":
    start_server()
