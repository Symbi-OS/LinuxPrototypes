import os
import socket
import sys
import time

def create_socketpair():
    # Create a pair of connected sockets
    parent_sock, child_sock = socket.socketpair()
    return parent_sock, child_sock

def child_process(sock, n, msg_size):
    # message = 'a' * msg_size
    message = b'a' * msg_size
    for _ in range(n):
        try:
            # Send a reply to parent
            # sock.sendall(message.encode())
            sock.sendall(message)
            # Try to receive message from parent in a non-blocking manner
            # msg = sock.recv(msg_size, socket.MSG_DONTWAIT).decode()
            msg = sock.recv(msg_size)
            # print(f"Child received: {msg[:10]}...")  # Print only the first 10 characters for brevity
        except BlockingIOError:
            # No data available to be read
            print("Blocking io error")
            pass
        except BrokenPipeError:
            # Handle the case when the parent socket is closed
            print("broken pipe error")
            break

def parent_process(sock, n, msg_size):
    # message = 'a' * msg_size
    message = b'a' * msg_size
    for _ in range(n):
        try:
            # Send a message to child
            sock.sendall(message)
            # Try to receive reply from child in a non-blocking manner
            # reply = sock.recv(msg_size, socket.MSG_DONTWAIT).decode()
            reply = sock.recv(msg_size)
            # print(f"Parent received: {reply[:10]}...")  # Print only the first 10 characters for brevity
        except BlockingIOError:
            # No data available to be read
            print("Blocking IO Error")
            pass
        except BrokenPipeError:
            # Handle the case when the child socket is closed
            print("Broken pipe error")
            break

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <n> <msg_size>")
        sys.exit(1)

    n = int(sys.argv[1])
    msg_size = int(sys.argv[2])

    parent_sock, child_sock = create_socketpair()

    pid = os.fork()
    if pid == 0:
        # Child process
        parent_sock.close()  # Close the parent's end of the socket in the child
        child_process(child_sock, n, msg_size)
        child_sock.close()  # Close the child's end of the socket in the child
    else:
        # Parent process
        child_sock.close()  # Close the child's end of the socket in the parent
        parent_process(parent_sock, n, msg_size)
        parent_sock.close()  # Close the parent's end of the socket in the parent

        # Wait for the child process to finish
        os.waitpid(pid, 0)

if __name__ == '__main__':
    main()