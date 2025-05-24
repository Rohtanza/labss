import socket

HOST = '127.0.0.1'
PORT = 65432

def print_menu():
    print("\nBank Account Menu:")
    print("1. Check Balance")
    print("2. Deposit Money")
    print("3. Withdraw Money")
    print("4. Exit")
    print("\nEnter your choice (1-4): ")

def handle_choice(choice):
    if choice == "1":
        return "BALANCE"
    elif choice == "2":
        amount = input("Enter amount to deposit (Rs.): ")
        return f"DEPOSIT {amount}"
    elif choice == "3":
        amount = input("Enter amount to withdraw (Rs.): ")
        return f"WITHDRAW {amount}"
    elif choice == "4":
        return "EXIT"
    else:
        return None

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    try:
        s.connect((HOST, PORT))
        print("Connected to bank server!")
        
        while True:
            print_menu()
            choice = input().strip()
            
            command = handle_choice(choice)
            if not command:
                print("Invalid choice! Please enter a number between 1 and 4.")
                continue
                
            # Send the command to server
            s.sendall(command.encode())
            
            # Get response
            response = s.recv(1024).decode()
            print("\nServer response:", response)
            
            if choice == "4":  # Exit
                break
                
    except ConnectionRefusedError:
        print("Could not connect to server. Make sure the server is running.")
    except Exception as e:
        print(f"An error occurred: {e}")

print("Disconnected from server.")

