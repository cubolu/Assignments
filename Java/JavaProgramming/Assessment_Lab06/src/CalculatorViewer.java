// Created by Jakub Lukac on 31.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 6 - Assessed Laboratory Work
//


import javax.swing.*;

public class CalculatorViewer {
    public static void main(String[] args) {
        // Creating frame
        CalculatorFrame frame = new CalculatorFrame();
        final int FRAME_WIDTH = 640;
        final int FRAME_HEIGHT = 320;

        // Setting frame attributes
        frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
        frame.setTitle("Calculator");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        // frame.setResizable(false);
        frame.setVisible(true);
    }
}
