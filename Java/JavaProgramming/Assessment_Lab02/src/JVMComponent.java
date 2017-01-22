// Created by Jakub Lukac on 06.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 2 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;

public class JVMComponent extends JComponent
{
    public void paintComponent(Graphics g)
    {
        // Recover Graphics2d
        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        JVMDiagram diagram = new JVMDiagram(g2);
        diagram.draw(10, 10, getWidth()-20, getHeight()-20);
    }
}
