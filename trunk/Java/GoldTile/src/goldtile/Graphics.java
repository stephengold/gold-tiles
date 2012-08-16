// File:     Graphics.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Graphics class for the Gold Tile Game
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
import java.awt.Color;
import java.awt.FontMetrics;
import java.awt.Point;
import java.awt.font.LineMetrics;
import java.awt.geom.Rectangle2D;

public class Graphics {
    public Color backgroundColor;
    public Color foregroundColor;
    private java.awt.Graphics context;
    
    private static Poly equilateral;
    private static Poly hexagon;

    public Graphics(java.awt.Graphics g) {
        backgroundColor = Color.WHITE;
        foregroundColor = Color.BLACK;
        context = g;
        
        // static polygons initialized on first instantiation of this class
        if (equilateral == null) {
            initializeEquilateral();
        }
        if (hexagon == null) {
            initializeHexagon();
        }
    }

    // methods
    
    public void drawHexagon(Rect bounds) {
        assert hexagon.size() == 6 : hexagon.size();

        drawPolygon(hexagon, bounds, false, Fill.YES);
    }
    
    public void drawPolygon(Poly poly, Rect bounds, 
            boolean invertFlag, Fill fill)
    {
        final int pointCount = poly.size();
        final int xPoints[] = new int[pointCount];
        final int yPoints[] = new int[pointCount];
        poly.getPoints(xPoints, yPoints, pointCount, bounds, invertFlag);
        
        if (fill == Fill.YES) {
            context.setColor(backgroundColor);
            context.fillPolygon(xPoints, yPoints, pointCount);
        }
        context.setColor(foregroundColor);
        context.drawPolygon(xPoints, yPoints, pointCount);
        
    }
    
    public void drawRect(Rect rect) {
        context.setColor(backgroundColor);
        context.fillRect(rect.x, rect.y, rect.width, rect.height);
    
        context.setColor(foregroundColor);
        context.drawRect(rect.x, rect.y, rect.width, rect.height);
    }
    
    public Rect drawRectangle(Point center, Area area) {
        final Rect result = new Rect(center, area);
        drawRect(result);
       
        return result; 
    }
    
    public void drawRoundedSquare(Point center, int edgeLength, 
            int arcDiameter)
    {
        assert arcDiameter >= 0 : arcDiameter;
        assert edgeLength > arcDiameter : edgeLength;

        final int leftX = center.x - edgeLength/2;
        final int topY = center.y - edgeLength/2;

        context.setColor(backgroundColor);
        context.fillRoundRect(leftX, topY, edgeLength, edgeLength, 
                arcDiameter, arcDiameter);

        context.setColor(foregroundColor);
        context.drawRoundRect(leftX, topY, edgeLength, edgeLength, 
                arcDiameter, arcDiameter);
    }

    // Draw a single line of text, centered in a rectangle.
    public void drawTextLine(Rect bounds, String text, String altText) {
        String displayText;
        int width = textWidth(text);

        if (altText == null || width <= bounds.width) {
            displayText = text;
        } else {
            displayText = altText;
            width = textWidth(altText);
        }
        
        final int height = textHeight(displayText);
        
        final int padX = bounds.width - width;
        final int padY = bounds.height - height;

        final int x = bounds.x + padX/2;
        final int y = bounds.y + textAscent(text) + padY/2;
        
        context.setColor(foregroundColor);
        context.drawString(displayText, x, y);
    }
    
    public void drawTextMultiline(Rect bounds, String text) {
        final int height = textHeight(text);
        final int width = textWidth(text);
        
        final int padX = bounds.width - width;
        final int padY = bounds.height - height;

        final int x = bounds.x + padX/2;
        final int y = bounds.y + textAscent(text) + padY/2;
        
        context.setColor(foregroundColor);
        context.drawString(text, x, y);        
    }
    
    private static void initializeEquilateral() {
        assert equilateral == null : equilateral;
        equilateral = new Poly();

        equilateral.add(0.0, 0.0);
        equilateral.add(0.5, 1.0);
        equilateral.add(1.0, 0.0);

        assert equilateral.size() == 3 : equilateral.size();
    }

    private static void initializeHexagon() {
        assert hexagon == null : hexagon;
        hexagon = new Poly();

         hexagon.add(0.0, 0.5);
         hexagon.add(0.25, 1.0);
         hexagon.add(0.75, 1.0);
         hexagon.add(1.0, 0.5);
         hexagon.add(0.75, 0.0);
         hexagon.add(0.25, 0.0);

        assert hexagon.size() == 6 : hexagon.size();
    }
    
    protected static Rect interiorEquilateral(Rect bounds, 
            boolean pointDownFlag)
    {
        FractionPair pairUlc;
        FractionPair pairBrc;
        if (pointDownFlag) {
            pairUlc = new FractionPair(0.291, 0.900);
            pairBrc = new FractionPair(0.709, 0.418);
        } else {
            pairUlc = new FractionPair(0.291, 0.582);
            pairBrc = new FractionPair(0.709, 0.100);
        }

        final Point ulc = pairUlc.interpolate(bounds, false);
        final Point brc = pairBrc.interpolate(bounds, false);

        return new Rect(ulc, brc);
    }
        
    protected static Rect interiorHexagon(Rect bounds) {
        final double den = 4 + 4*Global.SQRT_3;
        
        final FractionPair pairUlc = new FractionPair(
                2/den,
                (6 + 2*Global.SQRT_3)/den );
        final Point ulc = pairUlc.interpolate(bounds, false);

        final FractionPair pairBrc = new FractionPair(
                (2 + 4*Global.SQRT_3)/den, 
                (2*Global.SQRT_3 - 2)/den );
        final Point brc = pairBrc.interpolate(bounds, false);

        return new Rect(ulc, brc);
    }
    
    protected static Rect interiorRoundedSquare(Point center, 
            int edgeLength, int arcDiameter)
    {
         int leftX = center.x - edgeLength/2;
         int topY = center.y - edgeLength/2;

         // estimate the interior sqaure
         final int arcRadius = arcDiameter/2;
         final int pad = arcRadius - (int)(0.7 * arcRadius);
         topY += pad;
         leftX += pad;
         edgeLength -= 2*pad;
         
         return new Rect(topY, leftX, edgeLength, edgeLength);
    }
    
    private int textAscent(String text) {
        final FontMetrics metrics = context.getFontMetrics();
        final LineMetrics lm = metrics.getLineMetrics(text, context);
        final float ascent = lm.getAscent();
        
        return (int)(ascent + 0.99);        
    }
    
    private int textHeight(String text) {
        final FontMetrics metrics = context.getFontMetrics();
        final Rectangle2D bounds = metrics.getStringBounds(text, context);
        final double height = bounds.getHeight();
        
        return (int)(height + 0.99);
    }

    private int textWidth(String text) {
        final FontMetrics metrics = context.getFontMetrics();
        final Rectangle2D bounds = metrics.getStringBounds(text, context);
        final double width = bounds.getWidth();
        
        return (int)(width + 0.99);
    }
}