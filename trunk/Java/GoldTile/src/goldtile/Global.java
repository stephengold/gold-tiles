// File:     Global.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Global class for the Gold Tile Game
/**
 * @author Stephen Gold
 */
// (c) Copyright 2012 Stephen Gold
// Distributed under the terms of the GNU General Public License

/*
This file is part of the Gold Tile Game.

The Gold Tile Game is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Gold Tile Game is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License
along with the Gold Tile Game.  If not, see <http://www.gnu.org/licenses/>.
*/

package goldtile;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Random;

public class Global {
    // console I/O
    
    private static BufferedReader in = new BufferedReader(
            new InputStreamReader(System.in));
    private static java.io.PrintStream out = System.out; 

    public static void disableConsole() {
        in = null;
        out = null;
    }
    
    public static void print(String string) {
        if (out != null) {
            out.print(string);
        }
    }
    
    public static String readLine() {
        assert in != null;
        
        try {
            String result = in.readLine();
            result = result.trim();
            return result;
            
        } catch (IOException exception) {
            throw new AssertionError();
        }
    }

    // pseudo-random generator   
    
    final private static Random generator = new Random();

    public static Fraction nextFraction() {
        return new Fraction(generator.nextDouble());
    }
    
    public static int nextInt(int n) {
        return generator.nextInt(n);
    }  

    public static void reseedGenerator(long seed) {
        generator.setSeed(seed);
    }

    // math
    
    final public static double SQRT_3 = Math.sqrt(3);
    
    public static boolean isEven(int x) {
        return (x & 0x1) == 0;
    }
    
    public static boolean isOdd(int x) {
        return (x & 0x1) != 0;
    }    
}
