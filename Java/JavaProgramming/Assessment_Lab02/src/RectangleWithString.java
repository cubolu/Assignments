// Created by Jakub Lukac on 15.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 2 - Assessed Laboratory Work
//

import java.awt.*;
import java.awt.font.FontRenderContext;
import java.awt.font.TextLayout;
import java.util.ArrayList;

public class RectangleWithString {

    private Graphics2D g2;
    private Font font;
    private FontMetrics fontMetrics;
    private Color fontColor;
    private Color backgroundColor;

    private FontRenderContext frc;
    private int spaceWidth;
    private int textHeight;

    private double PADDING = 0.1;

    /**
     * Constructs object which could draw the text with specific rectangular background.
     *
     * @param g2 graphic context
     * @param font specify font of the text
     * @param fontColor colour of the font
     * @param backgroundColor colour of the rectangular background
     */
    public RectangleWithString(Graphics2D g2, Font font, Color fontColor, Color backgroundColor) {
        this.g2 = g2;
        this.font = font;
        this.fontMetrics =  g2.getFontMetrics(font);
        this.fontColor = fontColor;
        this.backgroundColor = backgroundColor;
        this.frc = g2.getFontRenderContext();
        this.spaceWidth = fontMetrics.charWidth(' ');
        this.textHeight = fontMetrics.getHeight();
    }

    /**
     * Allows to change the font of text which will be drawn.
     *
     * @param font specify new font
     */
    public void setFont(Font font) {
        this.font = font;
        this.fontMetrics =  g2.getFontMetrics(font);
        this.frc = g2.getFontRenderContext();
        this.spaceWidth = fontMetrics.charWidth(' ');
        this.textHeight = fontMetrics.getHeight();
    }

    /**
     * Draws the text with specific rectangular background, the text is properly wrapped into lines.
     *
     * @param xLeft x coordinate of drawing area
     * @param yTop y coordinate of drawing area
     * @param width width of drawing area
     * @param height height of drawing area
     * @param text actual string which will be drawn (new lines special character not supported)
     */
    public void draw(int xLeft, int yTop, int width, int height, String text) {
        Color defaultColor = g2.getColor();
        Font defaultFont = g2.getFont();

        // Drawing background - 3D rectangle
        g2.setColor(backgroundColor);
        g2.fill3DRect(xLeft, yTop, width, height, true);

        // Drawing text
        g2.setColor(fontColor);
        g2.setFont(font);

        // wrapping lines to fit to rectangle
        ArrayList<TextLayout> layouts = textWrapper((int)(width-PADDING*width), text);
        double baselineX;
        double baselineY = yTop + (height - layouts.size()*textHeight)/2 - fontMetrics.getDescent();  // position in centre of rectangle
        // drawing text line by line
        for (TextLayout layout : layouts) {
            baselineX = xLeft + (width - layout.getBounds().getWidth())/2;  // position in centre of line
            baselineY += textHeight;
            layout.draw(g2, (float) baselineX, (float) baselineY);
        }

        // Restore defaults
        g2.setColor(defaultColor);
        g2.setFont(defaultFont);
    }

    /**
     * Wraps the original text to lines based on width constrain.
     *
     * @param widthLine maximal width of line
     * @param text the original text
     * @return text layouts which one represents one line to be drawn
     */
    private ArrayList<TextLayout> textWrapper(int widthLine, String text) {
        ArrayList<TextLayout> wrappedWords = new ArrayList<>();

        if (text.isEmpty())
            return wrappedWords;

        // Splitting text to words
        String[] words = text.split("\\s");

        int wordWidth;
        int actualWidthOfLine = 0;
        String actualTextOfLine = "";
        for (String word : words) {
            // Finding out width of word
            wordWidth = fontMetrics.stringWidth(word);
            actualWidthOfLine += wordWidth;

            // Checking if it could fit in actual line
            if (actualWidthOfLine > widthLine) {
                // ending actual line
                TextLayout layout = new TextLayout(actualTextOfLine, font, frc);
                wrappedWords.add(layout);

                // initialize new line
                actualWidthOfLine = wordWidth;
                actualTextOfLine = word + " ";
            } else {
                // adding word to actual line
                actualTextOfLine += word + " ";
            }
            actualWidthOfLine += spaceWidth;
        }
        // Ending actual line buffer
        TextLayout layout = new TextLayout(actualTextOfLine, font, frc);
        wrappedWords.add(layout);

        return wrappedWords;
    }
}
