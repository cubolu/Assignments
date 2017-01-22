// Created by Jakub Lukac on 14.11.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 6 - Assessed Laboratory Work
//


import javax.swing.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.ParseException;

public class CalculatorFrame extends JFrame{

    private JTextField display;
    private Calculator calculator;
    private double memory;
    private DecimalFormat format;

    /**
     * Constructor.
     */
    public CalculatorFrame() {
        // Create UI
        setLayout(new BorderLayout());
        add(createTextField(), BorderLayout.NORTH);
        add(createKeypad(), BorderLayout.CENTER);

        // Create menu
        setJMenuBar(createMenu());

        // Create calculator
        calculator = new Calculator();

        // Initialize memory
        memory = 0.0;

        // Enable digit grouping by default
        DecimalFormatSymbols symbols = new DecimalFormatSymbols();
        symbols.setGroupingSeparator(',');
        format = new DecimalFormat("", symbols);
        // format.applyPattern("#.##"); // set two decimal places precision
        format.setGroupingUsed(true);
        format.setGroupingSize(3);
    }

    /**
     * Create calculator display with default string "0".
     * @return the text field
     */
    private Component createTextField() {
        display = new JTextField("0");
        display.setHorizontalAlignment(JTextField.RIGHT);
        return display;
    }

    /**
     * Create all calculator buttons.
     * @return the panel which includes all buttons
     */
    private Component createKeypad() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));

        // Create modification button; BS, CE, C
        panel.add(createModifications(), BorderLayout.NORTH);

        // Create memory, numbers and operators buttons
        JPanel lowerGrid = new JPanel(new GridLayout(1, 3, 10, 10));
        lowerGrid.add(createMemoryButtons());
        lowerGrid.add(createNumberButtons());
        lowerGrid.add(createOperatorButtons());
        panel.add(lowerGrid, BorderLayout.CENTER);

        return panel;
    }

    /**
     * Create modification buttons group, also define a listener for these buttons.
     * @return the panel with modification buttons
     */
    private Component createModifications() {
        JPanel panel = new JPanel(new GridLayout(1, 3, 10, 10)); // grid 4x3 with the 10px margin around each element
        panel.setBorder(new EmptyBorder(0, 0, 10, 0)); // BorderLayout correction

        // Create buttons
        String[] buttonsLabels = {
                "BS", "CE", "C"
        };

        ActionListener modificationListener = actionEvent -> {
            // Define modification rules
            switch (actionEvent.getActionCommand()) {
                case "BS":
                    // Backspace
                    if (!display.getText().isEmpty()) {
                        display.setText(display.getText().substring(0, display.getText().length() - 1));
                        updateNumberFormat();
                    }
                    break;
                case "CE":
                    // Reset display
                    display.setText("");
                    break;
                case "C":
                    // Reset calculator
                    calculator.setResult(0.0);
                    calculator.setLastOperator("=");
                    calculator.setStart(true);
                    // Reset display
                    display.setText("0");
                    break;
            }
        };

        createButtons(panel, modificationListener, buttonsLabels);

        return panel;
    }

    /**
     * Create memory buttons group, also define a listener for these buttons.
     * @return the panel with memory buttons
     */
    private Component createMemoryButtons() {
        JPanel panel = new JPanel(new GridLayout(4, 1, 10, 10)); // grid 4x1 with the 10px margin around each element
        // Create buttons
        String[] buttonsLabels = {
                "MC",
                "MR",
                "MS",
                "M+"
        };

        ActionListener memoryListener = actionEvent -> {
            // Define constrains and rules
            switch (actionEvent.getActionCommand()) {
                case "MC":
                    memory = 0.0;
                    break;
                case "MR":
                    setNumber(memory);
                    break;
                case "MS":
                    if (!display.getText().isEmpty())
                        memory = getNumber();
                    break;
                case "M+":
                    if (!display.getText().isEmpty())
                        memory += getNumber();
                    break;
            }
        };

        createButtons(panel, memoryListener, buttonsLabels);

        return panel;
    }

    /**
     * Create number pad for the calculator, also define a listener for each button.
     * @return the panel which includes the pad
     */
    private Component createNumberButtons() {
        JPanel panel = new JPanel(new GridLayout(4, 3, 10, 10)); // grid 4x3 with the 10px margin around each element
        // Create buttons
        String[] buttonsLabels = {
                "7", "8", "9",
                "4", "5", "6",
                "1", "2", "3",
                "0", "+/-", "."
        };

        ActionListener insertListener = actionEvent -> {
            // Define constrains and rules
            switch (actionEvent.getActionCommand()) {
                case "+/-":
                    if (!display.getText().isEmpty() && getNumber() != 0)
                        setNumber(getNumber() * -1);
                    break;
                case ".":
                    if (!display.getText().contains("."))
                        display.setText(display.getText() + ((JButton)actionEvent.getSource()).getText());
                    calculator.setStart(false);
                    break;
                default:
                    if (!display.getText().equals("0"))
                        display.setText(display.getText() + ((JButton)actionEvent.getSource()).getText());
                    else
                        display.setText(((JButton)actionEvent.getSource()).getText());
                    updateNumberFormat();
                    calculator.setStart(false);
                    break;
            }
        };

        createButtons(panel, insertListener, buttonsLabels);

        return panel;
    }

    /**
     * Create operator buttons group, also define a listener for these buttons.
     * @return the panel with operator buttons
     */
    private Component createOperatorButtons() {
        JPanel panel = new JPanel(new GridLayout(4, 2, 10, 10)); // grid 4x2 with the 10px margin around each element
        // Create buttons
        String[] buttonsLabels = {
                "/", "√",
                "*", "%",
                "-", "1/x",
                "+", "="
        };

        ActionListener operatorListener = actionEvent -> {
            if (!display.getText().isEmpty())
                // Define operator rules
                switch (actionEvent.getActionCommand()) {
                    case "-":
                        // Starting minus sign
                        if (calculator.getStart()) {
                            display.setText("-");
                            return;
                        }
                    case "/":
                    case "*":
                    case "+":
                        calculator.calculate(getNumber());
                        calculator.setLastOperator(actionEvent.getActionCommand());
                        // Reset display
                        display.setText("");
                        break;
                    case "=":
                        calculator.calculate(getNumber());
                        calculator.setLastOperator(actionEvent.getActionCommand());
                        // Show result
                        setNumber(calculator.getResult());
                        break;
                    case "1/x":
                    case "√":
                        calculator.setLastOperator(actionEvent.getActionCommand());
                        calculator.calculate(getNumber());
                        // Show result of unary operator
                        calculator.setLastOperator("=");
                        setNumber(calculator.getResult());
                        break;
                    case "%":
                        double base = calculator.getResult();
                        calculator.calculate(base * getNumber()/100);
                        // Show result
                        calculator.setLastOperator("=");
                        setNumber(calculator.getResult());
                        break;
                }
            // Empty display
            else if (actionEvent.getActionCommand().equals("=")) {
                calculator.setLastOperator(actionEvent.getActionCommand());
                // Show result
                setNumber(calculator.getResult());
            }
        };

        createButtons(panel, operatorListener, buttonsLabels);

        return panel;
    }

    /**
     * Create buttons with specific listener for each label and assign it to the panel.
     * @param p the panel which will includes all buttons
     * @param listener the action listener
     * @param labels the labels used to generate buttons
     */
    private void createButtons(JPanel p, ActionListener listener, String[] labels) {
        JButton button;
        for (String s: labels) {
            button = new JButton(s);
            button.setActionCommand(s);
            button.addActionListener(listener);
            p.add(button);
        }
    }

    /**
     * Create the menu bar for calculator.
     * @return the menu bar
     */
    private JMenuBar createMenu() {
        JMenuBar menu = new JMenuBar();
        menu.add(createEditMenuItem());
        menu.add(createViewMenuItem());
        menu.add(createHelpMenuItem());

        return menu;
    }

    /**
     * Create the "Edit" menu item and define its functionality.
     * @return the item
     */
    private JMenu createEditMenuItem() {
        JMenu menu = new JMenu("Edit");
        JMenuItem copy = new JMenuItem("Copy");
        JMenuItem paste = new JMenuItem("Paste");

        // Clipboard, declared as final to be allow in lambda functions
        final double[] clipboard = new double[1];

        // Create "Copy"
        copy.setMnemonic(KeyEvent.VK_C);
        copy.addActionListener(actionEvent -> {
            clipboard[0] = getNumber();
            paste.setEnabled(true);
        });
        menu.add(copy);

        // Create "Paste"
        paste.setEnabled(false);
        paste.setMnemonic(KeyEvent.VK_V);
        paste.addActionListener(actionEvent -> setNumber(clipboard[0]));
        menu.add(paste);

        return menu;
    }

    /**
     * Create the "View" menu item and define its functionality.
     * @return the item
     */
    private JMenu createViewMenuItem() {
        JMenu menu = new JMenu("View");
        JMenuItem item;

        // Create "Standard"
        item = new JMenuItem("Standard");
        menu.add(item);

        // Create "Scientific"
        item = new JMenuItem("Scientific");
        menu.add(item);

        menu.addSeparator();

        // Create "Digit grouping"
        JCheckBoxMenuItem itemCheckBox = new JCheckBoxMenuItem("Digit grouping", true);
        itemCheckBox.setMnemonic(KeyEvent.VK_D);
        itemCheckBox.addActionListener(actionEvent -> {
            format.setGroupingUsed(((JCheckBoxMenuItem)actionEvent.getSource()).getState());
            display.setText(display.getText().replaceAll(",", "")); // not loosing actual number when disabling the digit grouping
            updateNumberFormat();
        }
        );
        menu.add(itemCheckBox);

        return menu;
    }

    /**
     * Create the "Help" menu item and define its functionality.
     * @return the item
     */
    private JMenu createHelpMenuItem() {
        JMenu menu = new JMenu("Help");
        JMenuItem item;

        // Create "Help Topics"
        item = new JMenuItem("Help Topics");
        item.addActionListener(actionEvent -> {
            File file = new File("src/README.txt");
            try {
                Desktop.getDesktop().open(file);
            } catch (IOException e) {
                e.printStackTrace();
            }
        });
        menu.add(item);

        menu.addSeparator();

        // Create "About Calculator"
        item = new JMenuItem("About Calculator");
        item.addActionListener(actionEvent ->
                JOptionPane.showMessageDialog(this,
                "Calculator.\n\nCopyright © 2016 Jakub Lukac.")
        );
        menu.add(item);

        return menu;
    }

    /**
     * Show the number on the display, use the right format.
     * @param d the number
     */
    private void setNumber(Double d) {
        if (Double.isNaN(d))
            display.setText("NaN");
        else
            display.setText(format.format(d));
    }

    /**
     * Read the number from the display.
     * @return the number
     */
    private double getNumber() {
        try {
            return format.parse(display.getText()).doubleValue();
        } catch (ParseException e) {
            System.err.println("ParseException occurred.");
            return 0.0;
        }
    }

    /**
     * Refresh the number using the right format.
     */
    private void updateNumberFormat() {
        if (!display.getText().isEmpty()) {
            setNumber(getNumber());
        }
    }
}
