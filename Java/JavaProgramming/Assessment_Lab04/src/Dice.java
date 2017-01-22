// Created by Jakub Lukac on 24.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

/**
 * Generic dice interface.
 */
public interface Dice {
    int throwDice();
    int getValue();

    // Use for program testing
    void setValue(int v);
}
