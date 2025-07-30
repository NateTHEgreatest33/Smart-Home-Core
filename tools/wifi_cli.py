import socket
import argparse

def main():
    parser = argparse.ArgumentParser(description='TCP client for Pi Pico')
    parser.add_argument('--host', type=str, default='192.168.1.100', help='The IP address of the Pi Pico')
    parser.add_argument('--port', type=int, default=8080, help='The port the Pi Pico is listening on')
    args = parser.parse_args()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        print(f"Connecting to {args.host}:{args.port}...")
        try:
            s.connect((args.host, args.port))
            print("Connected!")
            while True:
                message = input("Enter message to send: ")
                if message.lower() == 'exit':
                    break
                s.sendall(message.encode('utf-8'))
                data = s.recv(1024)
                print('Received:', data.decode('utf-8'))
        except ConnectionRefusedError:
            print(f"Connection refused. Is the server running on {args.host}:{args.port}?")
        except Exception as e:
            print(f"An error occurred: {e}")

if __name__ == '__main__':
    main()
