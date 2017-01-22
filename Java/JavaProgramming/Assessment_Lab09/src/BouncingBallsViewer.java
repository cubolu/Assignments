// Created by Jakub Lukac on 04.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 9 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;

public class BouncingBallsViewer {

    public static void main(String[] args) {
        EventQueue.invokeLater(() -> {
            // Creating frame
            BouncingBallsFrame frame = new BouncingBallsFrame();
            final int FRAME_WIDTH = 600;
            final int FRAME_HEIGHT = 400;

            // Setting frame attributes
            frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
            frame.setTitle("Bouncing Balls");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            // frame.setResizable(false);
            frame.setVisible(true);
        });
    }
}
