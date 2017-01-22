// Created by Jakub Lukac on 08.01.2017.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.net.ConnectException;
import java.net.UnknownHostException;

public class ClientViewer {

    private static final String host = "localhost";
    private static final int port    = 8888;

    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            try {
                // Creating frame
                JFrame frame = new JFrame();
                final int FRAME_WIDTH = 400;
                final int FRAME_HEIGHT = 400;

                // Creating chat client
                ChatClient client = new ChatClient(host, port);

                // Creating login panel
                ClientLoginPanel loginPanel = new ClientLoginPanel(client);
                frame.add(loginPanel);
                loginPanel.setDefaultButton();

                // Setting frame attributes
                frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
                frame.setTitle("Chat Client");
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                // frame.setResizable(false);
                frame.setVisible(true);
            } catch (UnknownHostException | ConnectException e) {
                System.err.println("Cannot connect to " + host + ":" + port + ", " + e);
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
    }
}
