// Created by Jakub Lukac on 08.01.2017.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 11,12 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;
import java.net.SocketException;

public class ClientLoginPanel extends JPanel {

    private ChatClient client;

    private JTextField login;
    private JPasswordField password;
    private JButton submitButton;

    public ClientLoginPanel(ChatClient client) {
        this.client = client;

        // Build layout
        setLayout(new GridBagLayout());
        JPanel panel = new JPanel(new GridLayout(2, 1));
        panel.add(createForm());
        panel.add(createSubmitButton());
        add(panel);
    }

    /**
     * Create log in form.
     * @return the pane which consist of login and password fields
     */
    private Component createForm() {
        // Creating nick name field
        login = new JTextField(16);
        login.setText("jakub");
        JLabel loginLabel = new JLabel("login: ");
        loginLabel.setHorizontalAlignment(SwingConstants.RIGHT);

        // Creating password field
        password = new JPasswordField(16);
        password.setText("jakub");
        JLabel passwordLabel = new JLabel("password: ");
        passwordLabel.setHorizontalAlignment(SwingConstants.RIGHT);

        //Layout everything
        JPanel labelsPanel = new JPanel(new GridLayout(2, 1));
        labelsPanel.add(loginLabel);
        labelsPanel.add(passwordLabel);
        JPanel fieldsPanel = new JPanel(new GridLayout(2, 1));
        fieldsPanel.add(login);
        fieldsPanel.add(password);
        JPanel panel = new JPanel(new GridBagLayout());
        panel.add(labelsPanel);
        panel.add(fieldsPanel);

        return panel;
    }

    /**
     * Create the button which send credentials to the remote server.
     * @return the submit button
     */
    private Component createSubmitButton() {
        submitButton = new JButton("Login/Register");
        submitButton.addActionListener(actionEvent -> {
            try {
                switch (client.login(login.getText(), new String(password.getPassword()))) {
                    case SUCCESS:
                        JOptionPane.showMessageDialog(this, "Success! Correct password.");
                        changePanelToChat(getParent());
                        break;
                    case REGISTERED:
                        JOptionPane.showMessageDialog(this, "Success! You have been registered.");
                        changePanelToChat(getParent());
                        break;
                    case WRONG_PASSWORD:
                        JOptionPane.showMessageDialog(this,
                                "Invalid password. Try again.",
                                "Error Message",
                                JOptionPane.ERROR_MESSAGE);
                        break;
                }
            } catch (SocketException e) {
                e.printStackTrace();
                JOptionPane.showMessageDialog(this,
                        "Logging in failed. Application will be terminated.",
                        "Error Message",
                        JOptionPane.ERROR_MESSAGE);
                // Exit application
                System.exit(0);
            }
        });

        return submitButton;
    }

    /**
     * Switch the actual panel for one with the chat interface.
     * @param frame the parent frame
     */
    private void changePanelToChat(Container frame) {
        frame.removeAll();
        ClientChatPanel chatPanel = new ClientChatPanel(client);
        frame.add(chatPanel);
        chatPanel.setDefaultButton();
        frame.revalidate();
        frame.repaint();
    }

    /**
     * Allows clicking the login button with ENTER key.
     */
    public void setDefaultButton() {
        // Allow clicking the button with ENTER key
        getRootPane().setDefaultButton(submitButton);
    }
}
