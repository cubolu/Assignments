// Created by Jakub Lukac on 06.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 2 - Assessed Laboratory Work
//

import javax.swing.*;

public class JVMViewer
{
    public static void main(String[] args) {
        // Creating frame
        JFrame frame = new JFrame();
        final int FRAME_WIDTH = 800;
        final int FRAME_HEIGHT = 600;

        // Setting frame attributes
        frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
        frame.setTitle("JVMDiagram");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setResizable(false);

        // Adding component
        JVMComponent component = new JVMComponent();
        frame.add(component);

        frame.setVisible(true);
    }
}