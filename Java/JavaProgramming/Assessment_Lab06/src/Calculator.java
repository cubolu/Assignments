// source: Advanced Java ICP-2150 - Laboratories files

import java.awt.BorderLayout;
import java.awt.EventQueue;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;

import static java.lang.Double.NaN;

public class Calculator
{
   private double result;        // Holds accumulator value
   private String lastOperator;  // Stores last operator entered
   private boolean start;        // Indicates start of a computation

   public Calculator()
   {
      result = 0;
      lastOperator = "=";
      start = true;
   }
   
   public double getResult()
   {
      return result;
   }

   protected void setResult(double result)
   {
      this.result = result;
   }
   
   public boolean getStart()
   {
      return start;
   }
   
   public void setStart(boolean start)
   {
      this.start = start;
   }
   
   public String getLastOperator()
   {
      return lastOperator;
   }
   
   public void setLastOperator(String operator)
   {
      lastOperator = operator;
   }
   
   /**
    * Carries out the pending calculation.
    * @param x the value to be accumulated with the prior result.
    */
   public double calculate(double x)
   {
      if (lastOperator.equals("+")) result += x;
      else if (lastOperator.equals("-")) result -= x;
      else if (lastOperator.equals("*")) result *= x;
      else if (lastOperator.equals("/")) {
         if (x != 0)
            result /= x;
         else
            result = NaN;
      }
      else if (lastOperator.equals("=")) result = x;
      else if (lastOperator.equals("1/x")) {
         if (x != 0)
            result = 1/x;
         else
            result = NaN;
      }
      else if (lastOperator.equals("√")) result = Math.sqrt(x);
      //System.out.println("result = " + result);
      return result;
   }  
   
   /**
    * Returns string representation of calculator state.
    * @return calculator state
    */
   public String toString()
   {
      return getClass().getSimpleName() + 
      "[result = " + result + ", lastOperator = " + lastOperator
         + ", start = " + start + "]";
   }
}

   
    
  