// Created by Jakub Lukac on 17.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

import javax.swing.*;
import java.awt.*;

/**
 * Dice pair. Black and red dice.
 */
public class HighLowDices extends JPanel {

    private final int DICESIDEWIDTH = 100;

    public HighLowDices(Dice d0, Dice d1) {
        this.setLayout(new GridLayout(1, 4));
        this.add(new DiceFace(d0, DICESIDEWIDTH, Color.BLACK));
        this.add(new DiceFace(d1, DICESIDEWIDTH, Color.RED));
    }
}
