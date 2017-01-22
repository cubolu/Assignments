// Created by Jakub Lukac on 17.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

public class HighLowOptions extends JPanel {
    private JRadioButton highMode;
    private JRadioButton lowMode;
    private JRadioButton sevensMode;
    private ButtonGroup group;
    private JComboBox betAmount;
    private JLabel balanceLabel;
    private JButton playButton;

    private int balance;
    private ComboBoxModel[] betAmountModels;
    private Dice[] dices;

    private int STARTING_BALANCE = 50;

    /**
     * Construct UI interface for game options.
     *
     * @param d0 the dice object
     * @param d1 the dice object
     */
    public HighLowOptions(Dice d0, Dice d1) {
        balance = STARTING_BALANCE;
        dices = new Dice[]{d0, d1};

        setLayout(new GridLayout(1, 4));
        add(createModeOptions());
        add(createBetComboBox());
        add(createBalanceLabel());
        add(createPlayButton());
    }

    /**
     * Creates the radio buttons to select the game mode.
     *
     * @return the panel containing the radio buttons
     */
    private Component createModeOptions() {
        // Create "High" game option
        highMode = new JRadioButton("High");
        highMode.setMnemonic(KeyEvent.VK_H);
        // Create "Low" game option
        lowMode = new JRadioButton("Low");
        lowMode.setMnemonic(KeyEvent.VK_L);
        // Create "Sevens" game option
        sevensMode = new JRadioButton("Sevens");
        sevensMode.setMnemonic(KeyEvent.VK_S);

        // Handle selection event
        ActionListener listener = actionEvent -> playButton.setEnabled(true);
        highMode.addActionListener(listener);
        lowMode.addActionListener(listener);
        sevensMode.addActionListener(listener);

        // Add radio buttons to button group
        group = new ButtonGroup();
        group.add(highMode);
        group.add(lowMode);
        group.add(sevensMode);

        JPanel panel = new JPanel();
        // JPanel panel = new JPanel(new GridBagLayout());
        panel.setLayout(new GridLayout(3, 1));
        // panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        panel.add(highMode);
        panel.add(lowMode);
        panel.add(sevensMode);
        return panel;
    }

    /**
     * Creates the drop-down selection to select bet amount.
     *
     * @return the panel containing the drop-down selection box
     */
    private Component createBetComboBox() {
        // Values of combo box betAmount
        ComplexItem<Integer> tenBet = new ComplexItem<>(10, "£10");
        ComplexItem<Integer> fiveBet = new ComplexItem<>(5, "£5");
        ComplexItem<Integer> oneBet = new ComplexItem<>(1, "£1");
        betAmountModels = new ComboBoxModel[3];
        betAmountModels[0] = new DefaultComboBoxModel<>(new ComplexItem[]{ tenBet, fiveBet, oneBet });
        betAmountModels[1] = new DefaultComboBoxModel<>(new ComplexItem[]{ fiveBet, oneBet });
        betAmountModels[2] = new DefaultComboBoxModel<>(new ComplexItem[]{ oneBet });

        // Create combination betAmount
        betAmount = new JComboBox(betAmountModels[0]);
        betAmount.setPrototypeDisplayValue("£10");

        JPanel panel = new JPanel(new GridBagLayout());
        panel.add(betAmount);
        return panel;
    }

    /**
     * Creates the balance label to show the actual balance.
     *
     * @return the label
     */
    private Component createBalanceLabel() {
        balanceLabel = new JLabel();
        updateBalanceLabel();
        return balanceLabel;
    }

    /**
     * Creates the "throw" button.
     *
     * @return the panel containing the button
     */
    private Component createPlayButton() {
        playButton = new JButton("Throw dice");
        playButton.setEnabled(false);

        // Handle click event
        playButton.addActionListener(actionEvent -> {
            // Throw dices
            dices[0].throwDice();
            dices[1].throwDice();

            // Update game status after new throw
            updateBalance();
            updateBets();
            updateBalanceLabel();
            isGameFinished();

            // Repaint game
            getRootPane().repaint();
        });

        // Button layout
        JPanel panel = new JPanel(new GridBagLayout());
        panel.add(playButton);
        return panel;
    }

    /**
     * Update balance value which is based on throw result.
     */
    private void updateBalance() {
        int sum = dices[0].getValue() + dices[1].getValue();
        int bet = (int) ((ComplexItem)betAmount.getSelectedItem()).getItem();

        if ((sum > 7 && highMode.isSelected()) || (sum < 7 && lowMode.isSelected())) {
            // System.out.println("Win " + sum + " : " + bet);
            // 1:1
            balance += bet;
        } else if (sum == 7 && sevensMode.isSelected()) {
            // System.out.println("Win " + sum + " : " + bet);
            // 4:1
            balance += bet * 4;
        } else {
            // System.out.println("Lost");
            balance -= bet;
        }

        assert balance >= 0;
    }

    /**
     * Update possible bets, player cannot bet more then he/she has (the actual balance).
     */
    private void updateBets() {
        // Remember selection
        Object value = betAmount.getSelectedItem();

        if (balance > 9)
            betAmount.setModel(betAmountModels[0]);
        else if (balance > 4)
            betAmount.setModel(betAmountModels[1]);
        else if (balance > 0)
            betAmount.setModel(betAmountModels[2]);
        else
            betAmount.setEnabled(false);

        // Restore selected object, important when the model was changed
        betAmount.setSelectedItem(value);
    }

    /**
     * Update label which show the actual balance.
     */
    private void updateBalanceLabel() {
        balanceLabel.setText("Balance = £" + balance);
    }

    /**
     * Handle the end game situation.
     */
    private void isGameFinished() {
        String msg;

        if (balance > 99) {
            // Winnings
            msg = "You win the game with balance: £" + balance;
        } else if (balance < 1) {
            // Loss
            msg = "You lost all your balance!";
        } else {
            return;
        }

        // Show dialog
        int answer = JOptionPane.showConfirmDialog(
                getRootPane(),
                msg + "\nStart a new game?",
                "Game Over",
                JOptionPane.YES_NO_OPTION);
        // Resolve answer
        if (answer == 0) {
            // Start a new game
            // Reset dices
            dices[0].setValue(6);
            dices[1].setValue(6);
            // Reset balance
            balance = STARTING_BALANCE;
            // Reset interface options
            group.clearSelection();
            betAmount.setEnabled(true);
            playButton.setEnabled(false);
            updateBets();
            betAmount.setSelectedIndex(0);
            updateBalanceLabel();
        } else {
            System.exit(0);
        }
    }
}
