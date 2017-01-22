// Created by Jakub Lukac on 04.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 9 - Assessed Laboratory Work
//


import java.awt.*;
import java.util.Random;
import java.util.concurrent.ConcurrentLinkedQueue;
import javax.swing.*;
import javax.swing.border.EmptyBorder;

public class BouncingBallsFrame extends JFrame {

    private JPanel drawingBoard;
    private Random generator;
    private Timer timer;

    private ConcurrentLinkedQueue<BouncingBall> balls;
    private Thread collisionController;

    private final float DIAMETER = 20;
    private final int[] DELAYS = {300, 100, 50, 25, 5};

    /**
     * Constructor.
     */
    public BouncingBallsFrame() {
        setLayout(new BorderLayout());
        generator = new Random();
        timer = new Timer(DELAYS[2], actionEvent -> repaint());

        // Creating the drawing board
        drawingBoard = new JPanel();
        drawingBoard.setLayout(new BorderLayout());
        add(drawingBoard, BorderLayout.CENTER);

        balls = new ConcurrentLinkedQueue<>();
        collisionController = new Thread(new BallCollisionController(balls));
        collisionController.setDaemon(true);
        collisionController.start();

        // Creating the control panel
        JPanel controlPanel = new JPanel(new GridLayout(1, 2));
        controlPanel.setBorder(new EmptyBorder(10, 10, 10, 20));
        JPanel buttonsPanel = new JPanel(new GridBagLayout());
        // buttons
        buttonsPanel.add(createPlay());
        buttonsPanel.add(createPause());
        buttonsPanel.add(createStop());
        controlPanel.add(buttonsPanel);
        // the slider
        controlPanel.add(createSlider());
        add(controlPanel, BorderLayout.SOUTH);
    }

    /**
     * Create "Play" button.
     * @return button object
     */
    private Component createPlay() {
        JButton playButton = new JButton("Play");
        playButton.addActionListener(actionEvent -> {
            addBall();
            timer.start();
            revalidate();
            repaint();
        });

        return playButton;
    }

    /**
     * Create "Pause" button.
     * @return button object
     */
    private Component createPause() {
        JButton pauseButton = new JButton("Pause");
        pauseButton.setPreferredSize(new Dimension(90, 25));
        pauseButton.addActionListener(actionEvent -> {
            if (timer.isRunning()) {
                timer.stop();
                pauseButton.setText("Resume");
            }
            else {
                timer.start();
                pauseButton.setText("Pause");
            }
        });

        return pauseButton;
    }

    /**
     * Create "Stop" button.
     * @return button object
     */
    private Component createStop() {
        JButton stopButton = new JButton("Stop");
        stopButton.addActionListener(actionEvent -> {
            timer.stop();
            timer = new Timer(timer.getDelay(), actEvent -> repaint());
            // Interrupt all ball threads, this results in destroying the balls
            collisionController.interrupt();
            collisionController = new Thread(new BallCollisionController(balls));
            collisionController.setDaemon(true);
            collisionController.start();
        });

        return stopButton;
    }

    /**
     * Create slider bar.
     * @return slider
     */
    private Component createSlider() {
        int sliderMin = 1;
        int sliderMax = 5;

        JSlider slider = new JSlider(sliderMin, sliderMax, 3);
        slider.setMajorTickSpacing(1);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.addChangeListener(changeEvent -> {
            JSlider s = (JSlider) changeEvent.getSource();
            if (!s.getValueIsAdjusting())
                // Set a new delay
                timer.setDelay(DELAYS[s.getValue()-1]);
        });

        return slider;
    }

    /**
     * Add a new ball to the drawing board.
     */
    private void addBall()
    {
        BouncingBall ball = new BouncingBall(DIAMETER, generator);
        ball.setPosition(
                generator.nextInt((int) (drawingBoard.getWidth()-DIAMETER)),
                generator.nextInt((int) (drawingBoard.getHeight()-DIAMETER))
        );
        timer.addActionListener(ball);
        drawingBoard.add(ball);
        // Store ball in the queue
        balls.add(ball);
    }
}