import socket
import threading

HOST = '127.0.0.1'
PORT = 65432

# dictionary to store account balances
accounts = {}
# lock for thread safe operations
accounts_lock = threading.Lock()

def handle_client(conn, addr):
    print(f"[NEW CONNECTION] {addr} connected.")
    
    # creating a account for new connection with initial balance of 1000
    client_id = addr[1]  # using port as unique client ID
    with accounts_lock:
        if client_id not in accounts:
            accounts[client_id] = 1000.0
    
    with conn:
        while True:
            try:
                data = conn.recv(1024).decode()
                if not data:
                    break
                
                print(f"[{addr}] Received: {data}")
                
                # processing the commands
                command = data.strip().split()
                response = ""
                
                if command[0] == "BALANCE":
                    with accounts_lock:
                        response = f"Balance: Rs.{accounts[client_id]:.2f}"
                
                elif command[0] == "DEPOSIT":
                    amount = float(command[1])
                    with accounts_lock:
                        accounts[client_id] += amount
                        response = f"Deposited: Rs.{amount:.2f}. New balance: Rs.{accounts[client_id]:.2f}"
                
                elif command[0] == "WITHDRAW":
                    amount = float(command[1])
                    with accounts_lock:
                        if accounts[client_id] >= amount:
                            accounts[client_id] -= amount
                            response = f"Withdrawn: Rs.{amount:.2f}. New balance: Rs.{accounts[client_id]:.2f}"
                        else:
                            response = "Insufficient funds!"
                
                elif command[0] == "EXIT":
                    response = "Goodbye!"
                    conn.sendall(response.encode())
                    break
                
                else:
                    response = "Invalid command! Available commands: BALANCE, DEPOSIT <amount>, WITHDRAW <amount>, EXIT"
                
                conn.sendall(response.encode())
                
            except Exception as e:
                print(f"Error handling client {addr}: {e}")
                break
    
    print(f"[CONNECTION CLOSED] {addr} disconnected.")

def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"[LISTENING] Server is listening on {HOST}:{PORT}")
    print("Server started. Waiting for clients...")

    while True:
        conn, addr = server.accept()
        thread = threading.Thread(target=handle_client, args=(conn, addr))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")

if __name__ == "__main__":
    start_server()

