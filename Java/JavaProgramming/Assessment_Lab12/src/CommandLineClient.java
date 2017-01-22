// Created by Jakub Lukac on 11.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import java.net.*;
import java.io.*;
import java.util.Scanner;

public class CommandLineClient {

    public static void main(String[] args) {
        if (args.length != 2)
            System.out.println("Usage: java ChatServer host port");
        else {
            ChatClient client = null;
            try {
                client = new ChatClient(args[0], Integer.parseInt(args[1]));

                Scanner in = new Scanner(System.in);

                // Provide login information
                while (!client.isLogged()) {
                    System.out.printf("login: ");
                    String nick = in.nextLine();
                    System.out.printf("password: ");
                    String password = in.nextLine();
                    switch (client.login(nick, password)) {
                        case SUCCESS:
                            System.out.println("Successful log in.");
                            break;
                        case REGISTERED:
                            System.out.println("Successful registration.");
                            break;
                        case WRONG_PASSWORD:
                            System.out.println("Incorrect password, please try again...");
                            break;
                    }
                }

                // Receive messages in another thread
                ChatClient finalClient = client;
                Thread t = new Thread(() -> {
                    while (true) {
                        String s = finalClient.readLine();
                        if (s == null)
                            break;  // cannot read from socket

                        System.out.println("\r" + s);
                        System.out.printf(">> ");
                        System.out.flush();
                    }
                });
                t.setDaemon(true);
                t.start();

                // Send messages
                String line = "";
                System.out.printf(">> ");
                while (!(line.equals(".bye")) && in.hasNextLine()) {
                    System.out.printf(">> ");
                    line = in.nextLine();
                    client.send(line);
                }
            } catch (UnknownHostException | ConnectException e) {
                System.err.println("Cannot connect to " + args[0] + ":" + args[1] + ", " + e);
            } catch (SocketException e) {
                System.err.println(e);
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                System.out.println("Closing connection ...");
                if (client != null)
                    try {
                        client.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
            }
        }
    }
}
