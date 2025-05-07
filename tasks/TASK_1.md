# Task ID: 1
# Title: Setup Socket Manager
# Status: pending
# Dependencies: None
# Priority: high
# Description: Create the Socket Manager component to create and manage server sockets, accept new client connections, and maintain a list of active connections.
# Details:
Create a SocketManager class that handles:
1. Socket creation with socket()
2. Binding to specified ports/addresses with bind()
3. Listening for connections with listen()
4. Setting socket options with setsockopt() (e.g., SO_REUSEADDR)
5. Accepting new connections with accept()
6. Setting sockets to non-blocking mode with fcntl()

Implement proper error handling and resource cleanup. The class should support creating multiple server sockets for different host:port combinations.

# Test Strategy:
Test socket creation, binding to different ports, accepting connections using telnet or basic HTTP clients. Verify error handling by attempting to bind to already-in-use ports.

# Subtasks:
## 1. Implement Socket Wrapper Class [pending]
### Dependencies: None
### Description: Create a basic Socket class that encapsulates the raw socket operations and provides error handling
### Details:
Implementation steps:
1. Create a Socket class that encapsulates a socket file descriptor
2. Implement constructor that initializes member variables
3. Implement a method to create a socket with socket() system call
4. Add proper error handling for socket creation failures
5. Implement a destructor for socket resource cleanup
6. Add methods to get socket file descriptor and status
7. Implement utility methods for setting socket options like SO_REUSEADDR
8. Add fcntl() wrapper to set socket to non-blocking mode

Testing approach:
- Create unit tests to verify socket creation
- Test error handling by forcing failure conditions
- Verify socket options are properly set
- Confirm resource cleanup works correctly

## 2. Implement Server Socket Binding and Listening [pending]
### Dependencies: 1.1
### Description: Extend the Socket class to support binding to addresses/ports and listening for connections
### Details:
Implementation steps:
1. Add methods to the Socket class for binding to an address:port using bind()
2. Implement proper error handling for bind failures
3. Add method to start listening for connections with listen()
4. Add configurable backlog parameter for listen()
5. Implement address resolution for both IPv4 and IPv6
6. Create utility methods to check if socket is bound and listening

Testing approach:
- Test binding to various address:port combinations
- Verify error handling for invalid addresses and already-in-use ports
- Test listening with different backlog values
- Verify status methods correctly report socket state

## 3. Implement Connection Acceptance [pending]
### Dependencies: 1.2
### Description: Add functionality to accept new client connections and manage connection information
### Details:
Implementation steps:
1. Create a ClientConnection class to represent accepted client connections
2. Implement accept() wrapper method that returns a ClientConnection object
3. Store client information (address, port) in the ClientConnection class
4. Add proper error handling for accept failures
5. Implement non-blocking accept functionality
6. Add methods to get client information and connection status

Testing approach:
- Test accepting connections using a test client
- Verify client information is correctly captured
- Test non-blocking accept behavior
- Verify error handling works correctly

## 4. Implement Socket Manager Core [pending]
### Dependencies: 1.1, 1.2, 1.3
### Description: Create the SocketManager class to manage multiple server sockets
### Details:
Implementation steps:
1. Create SocketManager class with data structures to track multiple server sockets
2. Implement method to create and configure a new server socket
3. Add methods to bind and listen on specified address:port combinations
4. Implement error handling for socket creation and configuration
5. Add methods to check if specific ports are in use
6. Create utility methods to iterate through managed sockets
7. Implement proper resource management and cleanup

Testing approach:
- Test creating multiple server sockets on different ports
- Verify error handling when creating invalid sockets
- Test resource management with deliberate socket closure
- Verify iteration through managed sockets works correctly

## 5. Implement Connection Management [pending]
### Dependencies: 1.3, 1.4
### Description: Add functionality to track and manage active client connections
### Details:
Implementation steps:
1. Extend SocketManager to maintain a collection of active client connections
2. Implement method to accept connections on any ready server socket
3. Add functionality to remove closed or disconnected clients
4. Create methods to iterate through active connections
5. Implement connection lookup by client address/port
6. Add statistics tracking (connection count, accept rate, etc.)
7. Implement proper synchronization if needed for thread safety

Testing approach:
- Test accepting multiple client connections
- Verify connection tracking works correctly
- Test client disconnection handling
- Verify statistics are correctly maintained
- Test lookup functionality with various client connections
