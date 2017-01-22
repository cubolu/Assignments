// Created by Jakub Lukac on 08.01.2017.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;
import java.net.SocketException;

public class ClientChatPanel extends JPanel {

    private ChatClient client;

    private JTextArea textArea;
    private JScrollBar verticalScrollBar;
    private JButton sendButton;

    public ClientChatPanel(ChatClient client) {
        this.client = client;

        // Build layout
        setLayout(new BorderLayout());
        add(createTextArea(), BorderLayout.CENTER);
        add(createTypeInterface(), BorderLayout.SOUTH);
    }

    /**
     * Create and initialize the message text area. Also start worker thread
     * for receiving messages from remote clients.
     * @return the text area component with scroll feature
     */
    private Component createTextArea() {
        textArea = new JTextArea("THIS IS YOUR CHAT FEED:\n");
        textArea.setEditable(false);
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);

        // Enable scrolling through messages
        JScrollPane scrollPane = new JScrollPane (textArea);
        verticalScrollBar = scrollPane.getVerticalScrollBar();

        // Receive messages in another thread
        Thread t = new Thread(() -> {
            while (true) {
                String s = client.readLine();
                if (s == null)
                    break;  // cannot read from socket

                textArea.append(s + "\n");
                verticalScrollBar.setValue(verticalScrollBar.getMaximum());
            }
        });
        t.setDaemon(true);
        t.start();

        return scrollPane;
    }

    /**
     * Create interface for sending messages to remote clients.
     * @return the panel with text field and send button
     */
    private Component createTypeInterface() {
        JTextField field = new JTextField("Hello!");
        sendButton = new JButton("SEND");
        sendButton.addActionListener(actionEvent -> {
            try {
                client.send(field.getText());
                textArea.append(">> " + field.getText() + "\n");
                verticalScrollBar.setValue(verticalScrollBar.getMaximum());
                field.setText("");
            } catch (SocketException e) {
                e.printStackTrace();
                JOptionPane.showMessageDialog(this,
                        "The remote side is disconnected. Application will be terminated.",
                        "Error Message",
                        JOptionPane.ERROR_MESSAGE);
                // Exit application
                System.exit(0);
            }
        });

        JPanel typePanel = new JPanel(new BorderLayout());
        typePanel.add(field, BorderLayout.CENTER);
        typePanel.add(sendButton, BorderLayout.EAST);

        return typePanel;
    }

    /**
     * Allows clicking the send button with ENTER key.
     */
    public void setDefaultButton() {
        getRootPane().setDefaultButton(sendButton);
    }
}
