// Modified by Jakub Lukac on 24.10.2016.
// source: Advanced Java ICP-2150 - Laboratories files
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;
import java.awt.geom.*;

/**
 * Component which includes dice face with dots.
 */
public class DiceFace extends JPanel {
    // Holds the seven possible dot positions on a standard dice
    private Ellipse2D.Double[] dots = new Ellipse2D.Double[7];

    private Dice dice;
    private int diceWidth;
    private Color diceBackground;

    /**
     * Construct dice face component.
     *
     * @param dice the dice object, source of actual dice value
     * @param width  background rectangle width
     * @param background color of background
     */
    public DiceFace(Dice dice, int width, Color background) {
        this.dice = dice;
        this.diceWidth = width;
        this.diceBackground = background;
    }

    /**
     * Recipe to paint dice face.
     *
     * @param graphics the graphics object
     */
    @Override
    protected void paintComponent(Graphics graphics) {
        // Recover Graphics2d
        Graphics2D g2 = (Graphics2D) graphics;
        g2.setRenderingHint(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        Color defaultColor = g2.getColor();

        int xLeft = this.getWidth()/2 - this.diceWidth/2;
        int yTop = this.getHeight()/2 - this.diceWidth/2;

        Rectangle box = new Rectangle(xLeft, yTop, this.diceWidth, this.diceWidth);
        makeDots(xLeft, yTop, this.diceWidth);

        // Set diceBackground
        g2.setColor(diceBackground);
        g2.fill(box);

        // White dots on box
        g2.setColor(Color.WHITE);

        // Draw dots
        int diceValue = dice.getValue();
        if (diceValue == 1)
            g2.fill(dots[0]);
        else if (diceValue == 2) {
            g2.fill(dots[1]);
            g2.fill(dots[2]);
        } else if (diceValue == 3) {
            g2.fill(dots[0]);
            g2.fill(dots[1]);
            g2.fill(dots[2]);
        } else if (diceValue == 4) {
            g2.fill(dots[1]);
            g2.fill(dots[2]);
            g2.fill(dots[3]);
            g2.fill(dots[4]);
        } else if (diceValue == 5) {
            g2.fill(dots[0]);
            g2.fill(dots[1]);
            g2.fill(dots[2]);
            g2.fill(dots[3]);
            g2.fill(dots[4]);
        } else if (diceValue == 6) {
            g2.fill(dots[1]);
            g2.fill(dots[2]);
            g2.fill(dots[3]);
            g2.fill(dots[4]);
            g2.fill(dots[5]);
            g2.fill(dots[6]);
        }

        // Restore default color
        g2.setColor(defaultColor);
    }

    /**
     * Compute dots coordinates.
     *
     * @param xLeft the bounding rectangular x coordinate
     * @param yTop the bounding rectangular y coordinate
     * @param side the bounding rectangular width
     */
    private void makeDots(int xLeft, int yTop, int side) {
        int w = side / 6;   // dot (ellipse) width
        int h = w;          // dot (ellipse) height

        dots[0] = new Ellipse2D.Double(xLeft + (2.5 * side) / 6,
                yTop + (2.5 * side) / 6, h, w);

        dots[1] = new Ellipse2D.Double(xLeft + (3.75 * side) / 6,
                yTop + (3.75 * side) / 6, h, w);

        dots[2] = new Ellipse2D.Double(xLeft + (1.25 * side) / 6,
                yTop + (1.25 * side) / 6, h, w);

        dots[3] = new Ellipse2D.Double(xLeft + (1.25 * side) / 6,
                yTop + (3.75 * side) / 6, h, w);

        dots[4] = new Ellipse2D.Double(xLeft + (3.75 * side) / 6,
                yTop + (1.25 * side) / 6, h, w);

        dots[5] = new Ellipse2D.Double(xLeft + (1.25 * side) / 6,
                yTop + (2.5 * side) / 6, h, w);

        dots[6] = new Ellipse2D.Double(xLeft + (3.75 * side) / 6,
                yTop + (2.5 * side) / 6, h, w);
    }
}
