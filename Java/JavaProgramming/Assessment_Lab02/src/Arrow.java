// Created by Jakub Lukac on 12.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 2 - Assessed Laboratory Work
//

import java.awt.*;

public class Arrow {

    private Graphics2D g2;

    /**
     * Possible Horizontal directions of arrow.
     */
    public enum DirectionH {
        RIGHT, LEFT, RIGHTLEFT,
    }

    /**
     * Possible Vertical directions of arrow.
     */
    public enum DirectionV {
        DOWN, UP, DOWNUP
    }

    public Arrow(Graphics2D g2) {
        this.g2 = g2;
    }

    /**
     * Draws horizontal arrow shape.
     *
     * @param xLeft x coordinate of bounding box
     * @param yTop y coordinate of bounding box
     * @param width width of bounding box
     * @param height height of bounding box
     * @param spineWidthFactor ratio of arrow spine to bounding box
     * @param headWidthFactor ratio of arrow head to bounding box
     * @param direction direction of arrow head(s)
     */
    public void drawHorizontal(int xLeft, int yTop, int width, int height,
                               double spineWidthFactor, double headWidthFactor,
                               DirectionH direction) {
        // Initializing variables
        int nVertices;
        switch (direction) {
            case LEFT:
            case RIGHT:
                nVertices = 9;
                break;
            case RIGHTLEFT:
            default:
                nVertices = 10;
                break;
        }
        int[] xVertices = new int[nVertices];
        int[] yVertices = new int[nVertices];
        int xRight = xLeft + width;
        int yBottom = yTop + height;
        int xMiddle = xLeft + width/2;
        int yMiddle = yTop + height/2;

        // Building the polygon
        //               4.0         /7.0                   /7.0       4.0
        //               | \        / |                    / |         | \
        // 7.1-6---------5  \      /  6---------5--3.1    /  6---------5  \
        // |                 3.0  8.0                |   8.0               3.0
        // 8.1-0---------1  /      \  0---------1--2.1    \  0---------1  /
        //               | /        \ |                    \ |         | /
        //               2.0         \9.0                   \9.0       2.0

        int pointIndex = 0;

        // point 0
        xVertices[pointIndex]   = (int) (xMiddle - width/2*headWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle - height/2*spineWidthFactor);
        // point 1
        xVertices[pointIndex]   = (int) (xMiddle + width/2*headWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle - height/2*spineWidthFactor);

        // Adding RIGHT head
        if (direction == DirectionH.RIGHT || direction == DirectionH.RIGHTLEFT) {
            // point 2.0
            xVertices[pointIndex]   = (int) (xMiddle + width/2*headWidthFactor);
            yVertices[pointIndex++] = yBottom;
            // point 3.0
            xVertices[pointIndex]   = xRight;
            yVertices[pointIndex++] = yMiddle;
            // point 4.0
            xVertices[pointIndex]   = (int) (xMiddle + width/2*headWidthFactor);
            yVertices[pointIndex++] = yTop;
        } else {
            // point 2.1
            xVertices[pointIndex]   = xRight;
            yVertices[pointIndex++] = (int) (yMiddle - height/2*spineWidthFactor);
            // point 3.1
            xVertices[pointIndex]   = xRight;
            yVertices[pointIndex++] = (int) (yMiddle + height/2*spineWidthFactor);

        }

        // point 5
        xVertices[pointIndex]   = (int) (xMiddle + width/2*headWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle + height/2*spineWidthFactor);
        // point 6
        xVertices[pointIndex]   = (int) (xMiddle - width/2*headWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle + height/2*spineWidthFactor);

        // Adding LEFT head
        if (direction == DirectionH.LEFT || direction == DirectionH.RIGHTLEFT) {
            // point 7.0
            xVertices[pointIndex]   = (int) (xMiddle - width/2*headWidthFactor);
            yVertices[pointIndex++] = yTop;
            // point 8.0
            xVertices[pointIndex]   = xLeft;
            yVertices[pointIndex++] = yMiddle;
            // point 9.0
            xVertices[pointIndex] = (int) (xMiddle - width/2*headWidthFactor);
            yVertices[pointIndex] = yBottom;
        } else {
            // point 7.1
            xVertices[pointIndex]   = xLeft;
            yVertices[pointIndex++] = (int) (yMiddle + height/2*spineWidthFactor);
            // point 8.1
            xVertices[pointIndex] = xLeft;
            yVertices[pointIndex] = (int) (yMiddle - height/2*spineWidthFactor);
        }

        // Drawing arrow as one polygon
        g2.fillPolygon(xVertices, yVertices, nVertices);
    }

    /**
     * Draws vertical arrow shape.
     *
     * @param xLeft x coordinate of bounding box
     * @param yTop y coordinate of bounding box
     * @param width width of bounding box
     * @param height height of bounding box
     * @param spineWidthFactor ratio of arrow spine to bounding box
     * @param headWidthFactor ratio of arrow head to bounding box
     * @param direction direction of arrow head(s)
     */
    public void drawVertical(int xLeft, int yTop, int width, int height,
                             double spineWidthFactor, double headWidthFactor,
                             DirectionV direction) {

        int nVertices;
        switch (direction) {
            case DOWN:
            case UP:
                nVertices = 9;
                break;
            case DOWNUP:
            default:
                nVertices = 10;
                break;
        }
        int[] xVertices = new int[nVertices];
        int[] yVertices = new int[nVertices];
        int xRight = xLeft + width;
        int yBottom = yTop + height;
        int xMiddle = xLeft + width/2;
        int yMiddle = yTop + height/2;

        // Building the polygon
        //   8.1--7.1          8.0            8.0
        //     |  |           /  \           /  \
        //     |  |          /    \         /    \
        //     0  6       9.0-0  6-7.0   9.0-0  6-7.0
        //     |  |           |  |           |  |
        // 2.0-1  5-4.0       1  5       2.0-1  5-4.0
        //    \    /          |  |          \    /
        //     \  /           |  |           \  /
        //      3.0         2.1--3.1          3.0

        int pointIndex = 0;

        // point 0
        xVertices[pointIndex]   = (int) (xMiddle - width/2*spineWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle - height/2*headWidthFactor);
        // point 1
        xVertices[pointIndex]   = (int) (xMiddle - width/2*spineWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle + height/2*headWidthFactor);

        // Adding DOWN head
        if (direction == DirectionV.DOWN || direction == DirectionV.DOWNUP) {
            // point 2.0
            xVertices[pointIndex]   = xLeft;
            yVertices[pointIndex++] = (int) (yMiddle + height/2*headWidthFactor);
            // point 3.0
            xVertices[pointIndex]   = xMiddle;
            yVertices[pointIndex++] = yBottom;
            // point 4.0
            xVertices[pointIndex]   = xRight;
            yVertices[pointIndex++] = (int) (yMiddle + height/2*headWidthFactor);
        } else {
            // point 2.1
            xVertices[pointIndex]   = (int) (xMiddle - width/2*spineWidthFactor);
            yVertices[pointIndex++] = yBottom;
            // point 3.1
            xVertices[pointIndex]   = (int) (xMiddle + width/2*spineWidthFactor);
            yVertices[pointIndex++] = yBottom;

        }

        // point 5
        xVertices[pointIndex]   = (int) (xMiddle + width/2*spineWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle + height/2*headWidthFactor);
        // point 6
        xVertices[pointIndex]   = (int) (xMiddle + width/2*spineWidthFactor);
        yVertices[pointIndex++] = (int) (yMiddle - height/2*headWidthFactor);

        // Adding UP head
        if (direction == DirectionV.UP || direction == DirectionV.DOWNUP) {
            // point 7.0
            xVertices[pointIndex]   = xRight;
            yVertices[pointIndex++] = (int) (yMiddle - height/2*headWidthFactor);
            // point 8.0
            xVertices[pointIndex]   = xMiddle;
            yVertices[pointIndex++] = yTop;
            // point 9.0
            xVertices[pointIndex] = xLeft;
            yVertices[pointIndex] = (int) (yMiddle - height/2*headWidthFactor);
        } else {
            // point 7.1
            xVertices[pointIndex]   = (int) (xMiddle + width/2*spineWidthFactor);
            yVertices[pointIndex++] = yTop;
            // point 8.1
            xVertices[pointIndex] = (int) (xMiddle - width/2*spineWidthFactor);
            yVertices[pointIndex] = yTop;
        }

        // Drawing arrow as one polygon
        g2.fillPolygon(xVertices, yVertices, nVertices);
    }
}
