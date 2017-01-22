// Created by Jakub Lukac on 04.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 9 - Assessed Laboratory Work
//


import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Ellipse2D;
import java.util.Random;

public class BouncingBall extends JComponent implements ActionListener {

    private Ellipse2D.Float ball;
    private float dx;
    private float dy;
    protected Color ballColor;

    private int VELOCITY_RATE = 3;

    /**
     * Constructor.
     * @param diameter a new ball diameter
     * @param gen random number generator
     */
    public BouncingBall(float diameter, Random gen) {
        ball = new Ellipse2D.Float(0, 0, diameter, diameter);
        dx = (gen.nextFloat()*2 - 1) * VELOCITY_RATE;
        dy = (gen.nextFloat()*2 - 1) * VELOCITY_RATE;
        ballColor = new Color(gen.nextFloat(), gen.nextFloat(), gen.nextFloat());
    }

    /**
     * Get the coordinates of ball centre.
     * @return coordinates of the centre
     */
    public DimensionDouble getCentre() {
        double d = getDiameter();
        double x = ball.getX() + d/2;
        double y = ball.getY() + d/2;
        return new DimensionDouble(x, y);
    }

    /**
     * Set a new location of the ball.
     * @param x new x coordinate
     * @param y new x coordinate
     */
    public void setPosition(double x, double y) {
        ball.setFrame(x, y, getDiameter(), getDiameter());
    }

    /**
     * Get the diameter.
     */
    public double getDiameter() {
        return ball.getWidth();
    }

    /**
     * Set a new diameter.
     * @param d new diameter
     */
    public void setDiameter(float d) {
        ball.setFrame(ball.getX(), ball.getY(), d, d);
    }

    /**
     * Paint actual position of ball.
     * @param graphics the graphics object
     */
    @Override
    protected void paintComponent(Graphics graphics) {
        // Recover Graphics2d
        Graphics2D g2 = (Graphics2D) graphics;
        g2.setRenderingHint(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON
        );

        // Set ball color
        g2.setColor(ballColor);
        // Draw the ball
        g2.fill(ball);
    }

    /**
     * Action listener, causing the ball motion.
     * @param actionEvent the event information
     */
    @Override
    public void actionPerformed(ActionEvent actionEvent) {
        ball.setFrame(ball.getX()+dx, ball.getY()+dy, getDiameter(), getDiameter());
        bounceControl();
    }

    /**
     * Checking for possible collision with drawing area edges.
     */
    private void bounceControl() {
        int windowWidth = getWidth();
        int windowHeight = getHeight();
        double x = ball.getX();
        double y = ball.getY();
        double d = getDiameter();

        // Check if ball is near edge
        // the left edge
        if (x <= 0) {
            ball.setFrame(0, y, d, d);
            dx = -dx;
        }
        // the right edge
        else if (x >= windowWidth-d) {
            ball.setFrame(windowWidth-d, y, d, d);
            dx = -dx;
        }
        // the top edge
        if (y <= 0) {
            ball.setFrame(x, 0, d, d);
            dy = -dy;
        }
        // the bottom edge
        else if (y >= windowHeight-d) {
            ball.setFrame(x, windowHeight-d, d, d);
            dy = -dy;
        }
    }

    /**
     * Remove ball from the drawing board.
     */
    public void destroy() {
        Container c = getParent();
        if (c != null) {
            c.remove(this);
            c.repaint();
        }
    }
}
