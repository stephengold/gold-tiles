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
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Point;
import java.awt.font.LineMetrics;
import java.awt.geom.Rectangle2D;

public class Graphics {
    // constants
    private final static int FONT_HEIGHT_DEFAULT = 24;
    private final static int FONT_SIZE_MAX = 14;
    private final static int FONT_SIZE_MIN = 0;
    private final static int FONT_SIZE_COUNT = FONT_SIZE_MAX + 1;

    // per-instance fields, sorted by type
    public Color backgroundColor;
    public Color foregroundColor;
    private java.awt.Graphics context;

    // static fields, sorted by type
    private static Font fonts[] = null;
    private static Poly equilateral = null;
    private static Poly hexagon = null;

    // constructors

    public Graphics(java.awt.Graphics graphics) {
        assert graphics != null;

        backgroundColor = Color.WHITE;
        foregroundColor = Color.BLACK;
        context = graphics;

        // static font table initialized on first instantiation of this class
        if (fonts == null) {
            initializeFonts();
        }

        setFontDefault();

        // static polygons initialized on first instantiation of this class
        if (equilateral == null) {
            initializeEquilateral();
        }
        if (hexagon == null) {
            initializeHexagon();
        }
    }

    // methods, sorted by name

    public void drawEquilateralTriangle(Rect bounds, boolean pointDownFlag) {
        assert bounds != null;
        assert equilateral != null;
        assert equilateral.size() == 3 : equilateral.size();

        drawPolygon(equilateral, bounds, pointDownFlag, Fill.YES);
    }

    public void drawEquilateralTriangle(Point center, Area area,
            boolean pointDownFlag)
    {
        assert center != null;
        assert area != null;

        final Rect bounds = new Rect(center, area);
        drawEquilateralTriangle(bounds, pointDownFlag);
    }

    public void drawHexagon(Rect bounds) {
        assert bounds != null;
        assert hexagon.size() == 6 : hexagon.size();

        drawPolygon(hexagon, bounds, false, Fill.YES);
    }

    public void drawHexagon(Point center, Area area) {
        assert center != null;
        assert area != null;

        final Rect bounds = new Rect(center, area);
        drawHexagon(bounds);
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

    public void drawTextLine(Rect bounds, String text) {
        assert bounds != null;
        assert text != null;

        drawTextLine(bounds, text, null);
    }

    // Draw a single line of text, centered in a rectangle.
    public void drawTextLine(Rect bounds, String text, String altText) {
        assert bounds != null;
        assert text != null;

        String displayText;
        int width = getTextWidth(text);

        if (altText == null || width <= bounds.width) {
            displayText = text;
        } else {
            displayText = altText;
            width = getTextWidth(altText);
        }

        final int height = getTextHeight(displayText);

        final int padX = bounds.width - width;
        final int padY = bounds.height - height;

        final int x = bounds.x + padX/2;
        final int y = bounds.y + getTextAscent(text) + padY/2;

        context.setColor(foregroundColor);
        context.drawString(displayText, x, y);
    }

    private int findFontSize(Area area) {
        assert area != null;

        for (int iSize = FONT_SIZE_MAX; iSize >= FONT_SIZE_MIN; iSize--) {
            setFontSize(iSize);
            if (getFontHeight() <= area.height &&
                    getFontWidth() <= area.width)
            {
                return iSize;
            }
        }

        throw new AssertionError(area);
    }

    private int findFontSize(int height) {
        assert height > 0;

        for (int iSize = FONT_SIZE_MAX; iSize >= FONT_SIZE_MIN; iSize--) {
            setFontSize(iSize);
            if (getFontHeight() <= height) {
                return iSize;
            }
        }

        throw new AssertionError(height);
    }

    public int getFontHeight() {
        return getTextHeight("Wy");
    }

    public int getFontWidth() {
        return getTextWidth("W");
    }

    private static int getNominalHeight(int size) {
        assert size >= FONT_SIZE_MIN : size;
        assert size <= FONT_SIZE_MAX : size;

        int result = 4*size;

        return result;
    }

    public Area getTextArea(String text) {
        assert text != null;

        final FontMetrics metrics = context.getFontMetrics();
        final Rectangle2D bounds = metrics.getStringBounds(text, context);
        final double height = bounds.getHeight();
        final double width = bounds.getWidth();

        return new Area((int)(width + 0.99), (int)(height + 0.99));
    }

    private int getTextAscent(String text) {
        assert text != null;

        final FontMetrics metrics = context.getFontMetrics();
        final LineMetrics lm = metrics.getLineMetrics(text, context);
        final float ascent = lm.getAscent();

        return (int)(ascent + 0.99);
    }

    public int getTextHeight(String text) {
        assert text != null;

        final FontMetrics metrics = context.getFontMetrics();
        final Rectangle2D bounds = metrics.getStringBounds(text, context);
        final double height = bounds.getHeight();

        return (int)(height + 0.99);
    }

    public int getTextWidth(String text) {
        assert text != null;

        final FontMetrics metrics = context.getFontMetrics();
        final Rectangle2D bounds = metrics.getStringBounds(text, context);
        final double width = bounds.getWidth();

        return (int)(width + 0.99);
    }

    private static void initializeEquilateral() {
        assert equilateral == null : equilateral;

        equilateral = new Poly();

        equilateral.add(0.0, 0.0);
        equilateral.add(0.5, 1.0);
        equilateral.add(1.0, 0.0);

        assert equilateral.size() == 3 : equilateral.size();
    }

    private void initializeFonts() {
        assert fonts == null;

        fonts = new Font[FONT_SIZE_COUNT];
        final String family = "Tahoma";

        for (int iSize = 0; iSize < FONT_SIZE_COUNT; iSize++) {
            final int height = getNominalHeight(iSize);
            final int pointSize = (82*height)/100;
            final Font font = new Font(family, Font.PLAIN, pointSize);
            fonts[iSize] = font;

            setFontSize(iSize);
            final int actual = getFontHeight();
            assert actual <= height;
        }
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

    protected static Rect interiorEquilateralTriangle(Rect bounds,
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

        final Point ulc = pairUlc.interpolate(bounds);
        final Point brc = pairBrc.interpolate(bounds);

        return new Rect(ulc, brc);
    }

    protected static Rect interiorHexagon(Rect bounds) {
        assert bounds != null;

        final double den = 4 + 4*Global.SQRT_3;

        final FractionPair pairUlc = new FractionPair(
                2/den,
                (6 + 2*Global.SQRT_3)/den );
        final Point ulc = pairUlc.interpolate(bounds);

        final FractionPair pairBrc = new FractionPair(
                (2 + 4*Global.SQRT_3)/den,
                (2*Global.SQRT_3 - 2)/den );
        final Point brc = pairBrc.interpolate(bounds);

        return new Rect(ulc, brc);
    }

    protected static Rect interiorRoundedSquare(Point center,
            int edgeLength, int arcDiameter)
    {
        assert center != null;
        assert edgeLength > 0;
        assert arcDiameter >= 0;

        int leftX = center.x - edgeLength/2;
        int topY = center.y - edgeLength/2;

        // estimate the interior sqaure
        final int arcRadius = arcDiameter/2;
        final int pad = arcRadius - (int)(0.7 * arcRadius);
        topY += pad;
        leftX += pad;
        edgeLength -= 2*pad;

        return new Rect(leftX, topY, edgeLength, edgeLength);
    }

    public void setFontArea(Area area) {
        assert area != null;

        final int size = findFontSize(area);
        setFontSize(size);
    }

    final public void setFontDefault() {
        setFontHeight(FONT_HEIGHT_DEFAULT);
    }

    public void setFontHeight(int height) {
        assert height > 0 : height;

        final int size = findFontSize(height);
        setFontSize(size);
    }

    private void setFontSize(int size) {
        assert size >= FONT_SIZE_MIN : size;
        assert size <= FONT_SIZE_MAX : size;

        context.setFont(fonts[size]);
    }
}