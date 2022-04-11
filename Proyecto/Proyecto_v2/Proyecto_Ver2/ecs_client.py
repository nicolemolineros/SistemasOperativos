import socket
import sys


def get_constants(prefix):
    """Create a dictionary mapping socket module
    constants to their names.
    """
    return {
        getattr(socket, n): n
        for n in dir(socket)
        if n.startswith(prefix)
    }

def send_Message(message):
    families = get_constants('AF_')
    types = get_constants('SOCK_')
    protocols = get_constants('IPPROTO_')

    # Create a TCP/IP socket
    sock = socket.create_connection(('localhost', 8888))

    # print('Family  :', families[sock.family])
    # print('Type    :', types[sock.type])
    # print('Protocol:', protocols[sock.proto])
    # print()

    try:

        # Send data
        message = str.encode(message)
 
        # print('sending {!r}'.format(message))
        sock.sendall(message)

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            # print('received {!r}'.format(data))

    finally:
        print('closing socket')
        sock.close()


def recieve():
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET,
                        socket.SOCK_STREAM)

    # Bind the socket to the port
    server_address = ('localhost', 9999)
    # print('starting up on {} port {}'.format(*server_address))
    sock.bind(server_address)

    # Listen for incoming connections
    sock.listen(1)

    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = sock.accept()
    try:
        print('connection from', client_address)

        # Receive the data in small chunks and retransmit it
        data = connection.recv(3000)
        
        # print('received {!r}'.format(data))
        if data:
            print('recieved correctly')
            # connection.sendall(data)
        else:
            print('no data from', client_address)

    finally:
        # Clean up the connection
        connection.close()

    return data.decode("utf-8")

# op = 1: crear contenedor
# op = 2: Listar contenedores
# op = 3: borrar contenedor
# op = 4: detener contenedor

def listar():
    send_Message("2 vacio")
    print(recieve())

def borrar(nombre):
    send_Message("3 " + nombre)

def crear(imagen):
    send_Message("1 " + imagen)

def detener(nombre):
    send_Message("4 " + imagen)


