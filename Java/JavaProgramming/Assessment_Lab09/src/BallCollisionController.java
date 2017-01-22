// Created by Jakub Lukac on 05.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 9 - Assessed Laboratory Work
//


import java.awt.*;
import java.util.concurrent.ConcurrentLinkedQueue;

import static java.lang.Thread.sleep;

public class BallCollisionController implements Runnable {
    private ConcurrentLinkedQueue<BouncingBall> balls;

    private int STOP_TIME = 5000;
    private int DEFAULT_PARTICLE_COUNT = 10;

    public BallCollisionController(ConcurrentLinkedQueue<BouncingBall> balls) {
        this.balls = balls;
    }

    /**
     * Periodically checking for possible collisions.
     */
    @Override
    public void run() {
        while (!Thread.interrupted()) {
            for (BouncingBall ball1 : balls) {
                for(BouncingBall ball2 : balls) {
                    // Check for possible collision
                    if (ball1 != ball2) {
                        if (checkCollision(ball1, ball2)) {
                            detonateBall(ball1);
                            detonateBall(ball2);
                        }
                    }
                }
            }
        }

        // Freeze balls for 5 seconds
        try {
            sleep(STOP_TIME);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // Remove all balls
        for (BouncingBall b : balls)
            b.destroy();

        balls.clear();
    }

    /**
     * Check if one ball is colliding with other one.
     * @param ball1 ball object
     * @param ball2 ball object
     * @return true if balls are in collision, false otherwise
     */
    private boolean checkCollision(BouncingBall ball1, BouncingBall ball2) {
        // Get the centre of each ball
        DimensionDouble centre1 = ball1.getCentre();
        DimensionDouble centre2 = ball2.getCentre();
        // Compute the distance between ball's centres
        double deltaX = centre1.getWidth() - centre2.getWidth();
        double deltaY = centre1.getHeight() - centre2.getHeight();
        double distance = Math.sqrt(deltaX*deltaX + deltaY*deltaY);

        return distance <= ball1.getDiameter();
    }

    /**
     * Replace a ball with the explosion animation.
     * @param ball ball object which will be replaced
     */
    private void detonateBall(BouncingBall ball) {
        // Create explosion animation for the ball
        Container board = ball.getParent();
        board.add(new Explosion(DEFAULT_PARTICLE_COUNT, ball.getCentre()));
        board.revalidate();
        board.repaint();
        // Remove the ball
        ball.destroy();
        balls.remove(ball);
    }
}
