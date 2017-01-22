//
// Advanced Java ICP-2150 - Laboratory 4 - Assessed Laboratory Work
//

/*
	Revised and tested 10/06/2010
*/

import java.util.Random;

public class ClassicDice implements Dice {
    private Random generator;
    private int sides;
    private int value;

    public ClassicDice(int noSides) {
        generator = new Random();
        sides = noSides;
        value = noSides;
    }

    public int throwDice() {
        value = 1 + generator.nextInt(sides);
        return value;
    }

    public int getValue() {
        return value;
    }

    // Use for program testing
    public void setValue(int v) {
        value = v;
    }
}
