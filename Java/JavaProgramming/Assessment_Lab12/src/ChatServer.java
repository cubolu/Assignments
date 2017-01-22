// Created by Jakub Lukac on 10.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import java.net.ServerSocket;
import java.net.Socket;
import java.io.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

public class ChatServer implements Runnable {

    /**
     * Log in states which are part of communication protocol.
     */
    public enum State {
        SUCCESS {
            @Override
            public String toString() {
                return "OK 0";
            }
        },
        REGISTERED {
            @Override
            public String toString() {
                return "OK 1";
            }
        },
        WRONG_PASSWORD {
            @Override
            public String toString() {
                return "ERROR";
            }
        },
    }

    private ServerSocket sSocket;
    private PrintStream output;

    private CopyOnWriteArrayList<ChatServerConnection> chatRoom;
    private ConcurrentHashMap<String, String> credentials;

    /**
     * Each connection is maintained by this class in separate thread.
     */
    private class ChatServerConnection implements Runnable {
        private Socket socket;
        private BufferedReader in;
        private String nickName;

        private ChatServerConnection(Socket socket) throws IOException {
            this.socket = socket;
            // Create input stream
            this.in = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));
        }

        /**
         * Concurrent part of server.
         */
        @Override
        public void run() {
            try {
                output.println("Accepting: " + socket);

                // Handle the log in of a client
                handleLogIn();

                // Route and display client's messages
                String line;
                while (true) {
                    line = in.readLine();
                    if (line == null)  // cannot read from socket
                        break;
                    else if (line.equals(".bye"))  // exit sequence
                        break;
                    else if (line.length() > 160) {
                        // Disconnecting due to limit
                        PrintWriter responseOutput;
                        responseOutput = new PrintWriter(socket.getOutputStream(), true);
                        responseOutput.println("Client " + nickName + " - too prolix!!!\nDisconnecting now ...");
                        output.println(nickName + ": Disconnecting now ...");
                        break;
                    }

                    String msg = nickName + ": " + line;
                    output.println(msg);  // output message to the server monitoring stream
                    routeMsg(msg);  // send message to others in the chat room
                }
            } catch (IOException e) {
                output.println(nickName + ": " + e);
            } finally {
                chatRoom.remove(this);
                output.println(nickName + ": Closing connection...");
                try {
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        /**
         * Allows the client to log in.
         * @throws IOException if the connection failed
         */
        private void handleLogIn() throws IOException {
            String line;
            String password;

            boolean logged = false;
            while (!logged) {
                // Read nick name
                line = in.readLine();
                if (line == null)  // cannot read from socket
                    throw new IOException("Logging in failed");
                nickName = line.trim();
                // Read password
                line = in.readLine();
                if (line == null)  // cannot read from socket
                    throw new IOException("Logging in failed");
                password = line.trim();

                // Check login credentials
                State response;
                if (!credentials.containsKey(nickName)) {
                    // Register a new user
                    credentials.put(nickName, password);
                    response = State.REGISTERED;
                    logged = true;
                } else if (credentials.get(nickName).equals(password)) {
                    // Successful log in
                    response = State.SUCCESS;
                    logged = true;
                } else {
                    response = State.WRONG_PASSWORD;
                }

                // Send response
                PrintWriter responseOutput;
                responseOutput = new PrintWriter(socket.getOutputStream(), true);
                responseOutput.println(response);
            }
        }

        /**
         * Route a message to others clients in chat room.
         * @param msg the message
         */
        private void routeMsg(String msg) {
            chatRoom.forEach(item -> {
                PrintWriter socketOutput;
                try {
                    if(item != this){
                        socketOutput = new PrintWriter(item.socket.getOutputStream(), true);
                        socketOutput.println(msg);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
        }
    }

    /**
     * Construct the server.
     * @param port the port which will be opened to accept the new connections
     * @param outputStream the server monitoring stream
     * @throws IOException if port is already in use or can't be opened
     */
    public ChatServer(int port, PrintStream outputStream) throws IOException {
        this.sSocket = new ServerSocket(port);
        this.output = outputStream;

        this.chatRoom = new CopyOnWriteArrayList<>();
        this.credentials = new ConcurrentHashMap<>();
    }

    /**
     * Main server process which accepts the new connection,
     * could be run directly or in separate thread.
     */
    @Override
    public void run() {
        while (true) {
            try {
                // Accept connection
                ChatServerConnection connection = new ChatServerConnection(sSocket.accept());
                chatRoom.add(connection);
                Thread t = new Thread(connection);
                t.setDaemon(true);
                t.start();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }
}

