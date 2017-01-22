// Created by Jakub Lukac on 10.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import java.net.Socket;
import java.net.SocketException;
import java.net.ConnectException;
import java.io.*;

public class ChatClient {
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;
    private boolean logged;

    /**
     * Construct the chat client backend object and connect to host:port.
     * @param host the address of a remote server
     * @param port the port of a remote server
     * @throws IOException if the connection is not established correctly
     */
    public ChatClient(String host, int port) throws IOException{
        // Create socket, and establish connection to host:port
        this.socket = new Socket(host, port);
        if (!this.socket.isConnected())
            throw new ConnectException("Connection is not established");
        // Create output stream with autoflush enabled
        this.out = new PrintWriter(socket.getOutputStream(), true);
        // Create input stream
        this.in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        this.logged = false;
    }

    /**
     * Allows log in or register to the remote server.
     * @param nick user's name
     * @param password user's password
     * @return the log in status
     * @throws SocketException if unable to connect to the remote server
     */
    public ChatServer.State login(String nick, String password) throws SocketException {
        send(nick);
        send(password);
        String response = readLine();
        if (response != null && response.equals(String.valueOf(ChatServer.State.SUCCESS))) {
            logged = true;
            return ChatServer.State.SUCCESS;
        } else if (response != null && response.equals(String.valueOf(ChatServer.State.REGISTERED))) {
            logged = true;
            return ChatServer.State.REGISTERED;
        } else {
            return ChatServer.State.WRONG_PASSWORD;
        }
    }

    public boolean isLogged() {
        return logged;
    }

    /**
     * Send a text message to the remote server which send it to remote clients.
     * @param msg the test message
     * @throws SocketException if unable to connect to the remote server
     */
    public void send(String msg) throws SocketException {
        if (out.checkError())
            throw new SocketException("The remote side is disconnected");
        out.println(msg);
    }

    /**
     * Read the text messages from remote clients.
     * @return the message or null if communication failed
     */
    public String readLine() {
        try {
            return in.readLine();
        } catch (IOException ignore) {
            // Socket was closed, return null
        }
        return null;
    }

    public void close() throws IOException {
        socket.close();
    }
}
