// Created by Jakub Lukac on 05.12.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 9 - Assessed Laboratory Work
//


import java.awt.geom.Dimension2D;

public class DimensionDouble extends Dimension2D {
    private double width;
    private double height;

    public DimensionDouble(double width, double height) {
        this.width = width;
        this.height = height;
    }

    @Override
    public double getWidth() {
        return width;
    }

    @Override
    public double getHeight() {
        return height;
    }

    @Override
    public void setSize(double width, double height) {

    }
}
