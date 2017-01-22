// Created by Jakub Lukac on 05.12.2016.
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

public class Explosion extends JComponent {
    private Particle[] particles;
    private int counter;

    private int DEFAULT_SPEED_DELAY = 40;
    private int ANIMATION_LENGTH = 40;

    /**
     * Particle class. Create small balls - particles.
     */
    private class Particle implements ActionListener {
        private Ellipse2D.Double part;
        private float dx;
        private float dy;

        private int DIAMETER_RANGE = 5;
        private int VELOCITY_RATE = 3;

        /**
         * Construct a simple particle near origin coordinates.
         * @param originX x coordinate of particle origin
         * @param originY y coordinate of particle origin
         * @param gen random number generator
         */
        private Particle(double originX, double originY, Random gen) {
            float diameter = (gen.nextFloat()*2 - 1) * VELOCITY_RATE + DIAMETER_RANGE;
            part = new Ellipse2D.Double(originX, originY, diameter, diameter);
            dx = (gen.nextFloat()*2 - 1) * VELOCITY_RATE;
            dy = (gen.nextFloat()*2 - 1) * VELOCITY_RATE;
        }

        /**
         * Get a actual shape.
         * @return elliptic shape
         */
        private Ellipse2D.Double getShape() {
            return part;
        }

        /**
         * Action listener, causing the particle motion.
         * @param actionEvent the event information
         */
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            part.setFrame(part.getX()+dx, part.getY()+dy, part.getWidth(), part.getHeight());
        }
    }

    /**
     * Constructor of an explosion animation.
     * @param nParticles number of explosion particles
     * @param dim coordinates of the explosion centre
     */
    public Explosion(int nParticles, DimensionDouble dim) {
        this(nParticles, dim.getWidth(), dim.getHeight());
    }

    /**
     * Constructor of an explosion animation.
     * @param nParticles number of explosion particles
     * @param coordX x coordinate of the explosion centre
     * @param coordY y coordinate of the explosion centre
     */
    public Explosion(int nParticles, double coordX, double coordY) {
        Random generator = new Random();

        // Create timer to animate the explosion
        Timer timer = new Timer(DEFAULT_SPEED_DELAY, actionEvent -> {
            repaint();
            ++counter;
            if (counter > ANIMATION_LENGTH) {
                ((Timer) actionEvent.getSource()).stop();
                destroy();
            }
        });
        counter = 0;

        // Create particles
        particles = new Particle[nParticles];
        for (int i = 0; i < this.particles.length; ++i) {
            Particle p = new Particle(coordX, coordY, generator);
            this.particles[i] = p;
            timer.addActionListener(p);
        }

        // Start the animation
        timer.start();
    }

    /**
     * Paint all particles of the explosion.
     * @param graphics the graphics object
     */
    @Override
    protected void paintComponent(Graphics graphics) {
        // Recover Graphics2d
        Graphics2D g2 = (Graphics2D) graphics;
        g2.setColor(Color.RED);
        for (Particle p : particles)
            g2.fill(p.getShape());
    }

    /**
     * Remove visible particles.
     */
    public void destroy() {
        Container c = getParent();
        if (c != null){
            c.remove(this);
            c.repaint();
        }
    }
}
