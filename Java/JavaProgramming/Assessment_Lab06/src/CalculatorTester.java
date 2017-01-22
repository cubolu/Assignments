// Created by Jakub Lukac on 31.10.2016.
// login: eeu893
// e-mail: eeu893@bangor.ac.uk
//
// Advanced Java ICP-2150 - Laboratory 6 - Assessed Laboratory Work
//


public class CalculatorTester {
    public static void main(String[] args) {
        System.out.println("Testing Calculator...\n");

        // Definition of different test cases
        String[][] calculations = new String[][]{
                { "1",  "+", "2"},
                { "1",  "+", "2", "+", "3"},
                { "1",  "+", "2", "*", "3"},
                { "8",  "/", "2"},
                { "4",  "-", "2"},
                { "-2",  "+", "2"},
        };

        // Execute the test cases
        calculatorTester(calculations);
    }

    /**
     * Run all test cases.
     * @param testEquations the definition of equations
     */
    private static void calculatorTester(String[][] testEquations) {
        int testCounter = 1;
        Calculator calculator;

        for (String[] equation: testEquations) {
            System.out.print("Test " + testCounter++ + ": ");
            calculator = new Calculator();
            runTest(calculator, equation);
        }
    }

    /**
     * Run the individual test.
     * @param c the calculator object
     * @param equation the definition of equation
     */
    private static void runTest(Calculator c, String[] equation) {
        System.out.print("Calculate");

        for (String s: equation) {
            System.out.print(" " + s);
            if (s.matches("([-+]|)[\\d.\\.]")) {
                // matches the number
                c.calculate(Double.parseDouble(s));
            } else {
                // matches the operator
                c.setLastOperator(s);
            }
        }

        System.out.println();
        System.out.println("result = " + c.getResult());
    }
}
