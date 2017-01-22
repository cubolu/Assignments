// Created by Jakub Lukac on 10.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import java.io.IOException;

public class CommandLineServer {

    public static void main(String[] args) {
        if (args.length != 1)
            System.out.println("Usage: java ChatServer port");
        else {
            try {
                Runnable server = new ChatServer(Integer.parseInt(args[0]), System.out);
                Thread tServer = new Thread(server);
                tServer.start();
                tServer.join();
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
            }

        }
    }
}
