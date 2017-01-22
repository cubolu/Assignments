// Created by Jakub Lukac on 06.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 2 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;

public class JVMDiagram extends JComponent
{
    private Graphics2D g2;

    public JVMDiagram(Graphics2D g2) {
        this.g2 = g2;
    }

    /**
     * Draws whole diagram component.
     *
     * @param xLeft x coordinate of drawing area
     * @param yTop y coordinate of drawing area
     * @param width width of drawing area
     * @param height height of drawing area
     */
    public void draw(int xLeft, int yTop, int width, int height) {
        Color defaultColor = g2.getColor();
        Stroke defaultStroke = g2.getStroke();

        int xRight = xLeft + width;
        int yBottom = yTop + height;
        Arrow arrowGenerator = new Arrow(g2);
        RectangleWithString rectangleWithTextBox = new RectangleWithString(
                g2,
                new Font("Gill Sans", Font.PLAIN, 26),
                Color.BLACK,
                Color.decode("#DCE6F2"));

        /**************************************************/

        // "Class loader sub system" box
        int xTopItem = (width)/2 - (width)/8;
        int yTopItem = yTop;
        int widthTopItem = width/4;
        int heightTopItem = height/3-62;

        // "Class loader sub system" box
        rectangleWithTextBox.draw(xTopItem, yTopItem,
                widthTopItem, heightTopItem, "Class loader sub system");

        /**************************************************/

        // Horizontal right arrow "Class files"
        g2.setFont(new Font("Gill Sans", Font.PLAIN, 18));
        g2.setColor(Color.BLACK);
        g2.drawString("Class files", xTopItem-widthTopItem+40, yTopItem+heightTopItem/2);
        // arrows colour
        g2.setColor(Color.decode("#215968"));
        arrowGenerator.drawHorizontal(xTopItem-widthTopItem, yTopItem+heightTopItem/2, widthTopItem, 36,
                0.3, 0.75, Arrow.DirectionH.RIGHT);

        /**************************************************/

        // Two arrows connecting "Class loader sub system" box with middle part
        int xTopMidItem = xLeft + width/2;
        int yTopMidItem = yTop + heightTopItem;
        int heightTopMidItem = 60;
        arrowGenerator.drawVertical(xTopMidItem-39, yTopMidItem, 36, heightTopMidItem,
                0.3, 0.2, Arrow.DirectionV.UP);
        arrowGenerator.drawVertical(xTopMidItem+3, yTopMidItem, 36, heightTopMidItem,
                0.3, 0.2, Arrow.DirectionV.DOWN);

        /**************************************************/

        drawDashedRect(xLeft, yTop+height/3, width, height/3);

        /**************************************************/

        // arrow colour
        g2.setColor(Color.decode("#215968"));

        // Two arrows connecting "Execution Engine" box
        int xBottomMidItem = xLeft + width/6;
        int yBottomMidItem = yTop + height*2/3+4;
        int heightBottomMidItem = heightTopMidItem;
        arrowGenerator.drawVertical(xBottomMidItem-39, yBottomMidItem, 36, heightBottomMidItem,
                0.3, 0.2, Arrow.DirectionV.UP);
        arrowGenerator.drawVertical(xBottomMidItem+3, yBottomMidItem, 36, heightBottomMidItem,
                0.3, 0.2, Arrow.DirectionV.DOWN);

        /**************************************************/

        int xBottomItem = xBottomMidItem-80;
        int yBottomItem = yBottomMidItem+heightBottomMidItem+2;
        int widthBottomItem = width/4;
        int heightBottomItem = height/3-60;

        // "Execution Engine" box
        rectangleWithTextBox.draw(xBottomItem, yBottomItem,
                widthBottomItem, heightBottomItem, "Execution Engine");
        /**************************************************/
        xBottomItem += widthBottomItem;
        // Horizontal right-left arrow
        arrowGenerator.drawHorizontal(xBottomItem, yBottomItem+heightBottomItem/2-18, widthBottomItem, 36,
                0.4, 0.8, Arrow.DirectionH.RIGHTLEFT);
        /**************************************************/
        xBottomItem += widthBottomItem;
        // "Native method interface" box
        rectangleWithTextBox.draw(xBottomItem, yBottomItem,
                heightBottomItem, heightBottomItem, "Native method interface");
        /**************************************************/
        // Arrow connecting "Native method interface" box
        xBottomMidItem = xBottomItem+heightBottomItem/2-18;
        arrowGenerator.drawVertical(xBottomMidItem, yBottomMidItem, 36, heightBottomMidItem,
                0.5, 0.4, Arrow.DirectionV.DOWNUP);
        /**************************************************/
        xBottomItem += heightBottomItem;
        // Horizontal left arrow
        arrowGenerator.drawHorizontal(xBottomItem, yBottomItem+heightBottomItem/2-18, 50, 36,
                0.35, 0.1, Arrow.DirectionH.LEFT);
        /**************************************************/
        xBottomItem += 52;
        // "Native method library" box
        rectangleWithTextBox.draw(xBottomItem, yBottomItem,
                heightBottomItem, heightBottomItem, "Native method library");

        /**************************************************/

        // Restore defaults
        g2.setColor(defaultColor);
        g2.setStroke(defaultStroke);
    }

    /**
     * Draws middle part of diagram, specifically the dashed rectangle and its content.
     *
     * @param xLeft x coordinate of drawing area
     * @param yTop y coordinate of drawing area
     * @param width width of drawing area
     * @param height height of drawing area
     */
    private void drawDashedRect(int xLeft, int yTop, int width, int height) {

        RectangleWithString rectangleWithTextBox = new RectangleWithString(
                g2,
                new Font("Gill Sans", Font.PLAIN, 26),
                Color.BLACK,
                Color.decode("#DCE6F2"));

        /**************************************************/

        g2.setColor(Color.BLACK);
        // Set dashed stroke
        float dash[] = { 10.0f };
        g2.setStroke(new BasicStroke(3.0f, BasicStroke.CAP_BUTT,
                BasicStroke.JOIN_MITER, 10.0f, dash, 0.0f));

        // Dashed rectangle
        g2.drawRect(xLeft, yTop, width, height);

        /**************************************************/

        // Boxes inside dashed rectangle
        int xRect = xLeft + 20;
        int yRect = yTop + 20;
        int widthRect = (width - 100)/5;
        rectangleWithTextBox.draw(xRect, yRect, widthRect, height-60, "Method area");
        xRect += 15 + widthRect;
        rectangleWithTextBox.setFont(new Font("Gill Sans", Font.PLAIN, 30));
        rectangleWithTextBox.draw(xRect, yRect, widthRect, height-60, "Heap");
        xRect += 15 + widthRect;
        rectangleWithTextBox.setFont(new Font("Gill Sans", Font.PLAIN, 26));
        rectangleWithTextBox.draw(xRect, yRect, widthRect, height-60, "Java stacks");
        xRect += 15 + widthRect;
        rectangleWithTextBox.draw(xRect, yRect, widthRect, height-60, "PC Registers");
        xRect += 15 + widthRect;
        rectangleWithTextBox.draw(xRect, yRect, widthRect, height-60, "Native method stacks");

        /**************************************************/

        // String inside dashed rectangle
        String bottomText = "Runtime data areas";
        g2.setFont(new Font("Gill Sans", Font.PLAIN, 20));
        FontMetrics fontMetrics =  g2.getFontMetrics(g2.getFont());
        int wordWidth = fontMetrics.stringWidth(bottomText);
        g2.drawString(bottomText, xLeft+(width - wordWidth)/2, yTop+height-5);
    }
}
