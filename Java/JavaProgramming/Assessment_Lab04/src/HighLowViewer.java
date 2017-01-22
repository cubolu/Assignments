// Created by Jakub Lukac on 17.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;

public class HighLowViewer {
    public static void main(String[] args) {
        // Creating frame
        JFrame frame = new JFrame();
        final int FRAME_WIDTH = 600;
        final int FRAME_HEIGHT = 400;

        // Setting frame attributes
        frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
        frame.setTitle("High Low Version 1.0");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setLayout(new BorderLayout());
        // frame.setResizable(false);

        // Creating dices
        Dice blackDice = new ClassicDice(6);
        Dice redDice = new ClassicDice(6);

        // Adding dices
        HighLowDices dicesDrawing = new HighLowDices(blackDice, redDice);
        frame.add(dicesDrawing, BorderLayout.CENTER);

        // Adding game control components
        HighLowOptions controls = new HighLowOptions(blackDice, redDice);
        frame.add(controls, BorderLayout.SOUTH);

        frame.setVisible(true);
    }
}