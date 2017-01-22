// Created by Jakub Lukac on 08.01.2017.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;
import java.io.*;

public class ServerViewer {

    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            // Creating frame
            JFrame frame = new JFrame();
            final int FRAME_WIDTH = 600;
            final int FRAME_HEIGHT = 400;

            // Creating text area for the monitoring server output
            JTextArea textArea = new JTextArea();
            textArea.setEditable(false);
            textArea.setLineWrap(true);
            textArea.setWrapStyleWord(true);
            // Enable scrolling through messages
            JScrollPane scrollPane = new JScrollPane (textArea);
            JScrollBar verticalScrollBar = scrollPane.getVerticalScrollBar();
            frame.add(scrollPane);

            // Creating server
            try {
                // Pipe the custom print stream with the text area
                PrintStream printStream = new PrintStream(new OutputStream() {
                    @Override
                    public void write(int i) throws IOException {
                        // redirects data to the text area
                        textArea.append(String.valueOf((char)i));
                        verticalScrollBar.setValue(verticalScrollBar.getMaximum());
                        // scrolls the text area to the end of data
                        // textArea.setCaretPosition(textArea.getDocument().getLength());
                    }
                });
                Runnable server = new ChatServer(8888, printStream);
                Thread tServer = new Thread(server);
                tServer.start();
            } catch (IOException e) {
                e.printStackTrace();
            }

            // Setting frame attributes
            frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
            frame.setTitle("Server Monitoring Window");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            // frame.setResizable(false);
            frame.setVisible(true);
        });
    }
}