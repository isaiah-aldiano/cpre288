# Author: Phillip Jones (phjones@iastate.edu)
# Date: 07/12/2023
# Description: Simple "Echo" server, or "Chat" server example

# General Python tutorials (W3schools):  https://www.w3schools.com/python/

# Time library (e.g. sleep(), https://realpython.com/python-time-module/)
import time    

# Socket library:  https://realpython.com/python-sockets/  
# See: Background, Socket API Overview, and TCP Sockets  
import socket

# TCP Socket BEGIN
HOST = "127.0.0.1"  # Standard loopback interface IP address (localhost)
PORT = 65432  # Port to listen on (non-privileged ports are > 1023)

print("Creating Socket object ")
cybot_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Create a socket object

print("Binding socket object to Host: " + HOST + " and Port: " + str(PORT) )
cybot_server_socket.bind((HOST, PORT))  # Bind server to a host and port

print("Listening for a client to connect")
cybot_server_socket.listen()            # Listen for a client to connect to this server

conn, addr = cybot_server_socket.accept()  # Accept a client request to connect to this server, and create a connection
print("Socket Connection from: " + str(addr))  # Display address of client that just connected
# TCP Socket END

while True:  # Loop recieving/sending data from/to client until client disconnects, or error causes data to be 0 (i.e., no data)

	try: # Receive data from client.
		data = conn.recv(1024)  # Recieve byte array, will not accept a data packet greater than 1024 bytes		

	except socket.error as msg:  # Client has disconnected, or there has been some type of error while recv waits for data
		print("Connection closed: " + str(msg) + "\n")
		break

	print("Recieved from client: " + data.decode()) # Convert message from bytes to String (i.e., decode), then print
	
	data_to_client = data.decode()  # Echo Server: option for echoing data back to client, or 
	#data_to_client = input('Enter data to sent to the cliet -> ') + '\n'  # Chat Server: option for interactively choosing what data to send to the client
	send_message = input("Enter a message (enter quit to exit):") + '\n'

	conn.send(send_message.encode())  # Convert String to bytes (i.e., encode), and send data to the client
	print("Sent to client:" + data_to_client)

print("Server exiting\n")
time.sleep(3) # Sleep for 3 seconds
conn.close()  # close the Socket connection