// File:     Canvas.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Canvas class for the Gold Tile Game
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

import java.awt.Point;

public class Canvas extends Graphics {
    // constants
    private static Poly shapes[] = null;
    private static Poly targetArrow = null;
    final private static int TILE_POINTEDNESS = 3; // corners of square tiles

    // constructors

    public Canvas(java.awt.Graphics context) {
        super(context);

        if (shapes == null) {
            initializeShapes();
        }
        if (targetArrow == null) {
            initializeTargetArrow();
        }
    }

    // methods, sorted by name

    public void drawBlankTile(Point center, Area area, Color tileColor,
            boolean oddFlag)
    {
        final int width = area.width;
        assert Global.isEven(width) : width;

        final Color borderColor = Color.DARK_GRAY;
        foregroundColor = borderColor;
        backgroundColor = tileColor;

        if (getCellShape() == Shape.SQUARE) {
            assert area.isSquare();
            final int arcDiameter = width/TILE_POINTEDNESS;
            drawRoundedSquare(center, width, arcDiameter);
        } else {
            drawGridShape(center, area, oddFlag);
        }
    }

    public Rect drawCell(Point center, Area area,
                Color cellColor, Color gridColor, boolean oddFlag)
    {
        assert Global.isEven(area.width);

        backgroundColor = cellColor;
        foregroundColor = gridColor;
        drawGridShape(center, area, oddFlag);

        return interiorGridShape(center, area, oddFlag);
    }

    public void drawGridShape(Point center, Area area, boolean oddFlag) {
        switch (getCellShape()) {
            case HEXAGON:
                assert !oddFlag;
                drawHexagon(center, area);
                break;
            case SQUARE:
                assert area.isSquare();
                drawRectangle(center, area);
                break;
            case TRIANGLE:
                drawEquilateralTriangle(center, area, oddFlag);
                break;
            default:
                throw new AssertionError(getCellShape());
       }
    }

    private void drawMarking(Rect bounds, AttrMode displayMode, Attr attr,
            Color color)
    {
        switch (displayMode) {
            case SHAPE:
                foregroundColor = color;
                backgroundColor = color;

                final Poly shape = shapes[attr.intValue()];
                final Rect square = bounds.centerSquare();
                drawPolygon(shape, square, false, Fill.YES);
                break;

            case ABC:
            case NUMERAL:
            case RST:
                foregroundColor = color;

                final char ch = displayMode.attrToChar(attr);
                final String str = Character.toString(ch);
                //TODO useFont(bounds.height, bounds.width);
                drawTextLine(bounds, str, null);
                //TODO useFont(FONT_HEIGHT_DEFAULT);
                break;

            default:
                throw new AssertionError(displayMode);
        }
    }

    public void drawTargetArrow(Rect bounds) {
        drawPolygon(targetArrow, bounds, false, Fill.YES);
    }

    public Rect drawTile(Markings markings, Color tileColor, Point center,
            Area tileArea, boolean borderFlag, boolean oddFlag)
    {
        final int width = tileArea.width;
        assert Global.isEven(width) : width;

       if (borderFlag) {
           Color borderColor = Color.WHITE;
           if (tileColor == Color.DULL_GOLD) {
               borderColor = Color.GOLD;
           }
           backgroundColor = borderColor;
           foregroundColor = borderColor;
        } else {
            backgroundColor = tileColor;
            foregroundColor = Color.DARK_GRAY;
        }

        Rect interior;
        if (getCellShape() == Shape.SQUARE) {
            assert tileArea.isSquare();
            final int arcDiameter = width/TILE_POINTEDNESS;
            drawRoundedSquare(center, width, arcDiameter);
            if (borderFlag) {
                backgroundColor = tileColor;
                foregroundColor = tileColor;
                drawRoundedSquare(center, (width*7)/8, (arcDiameter*7)/8);
            }
            interior = interiorRoundedSquare(center, width, arcDiameter);
        } else {
            drawGridShape(center, tileArea, oddFlag);
            if (borderFlag) {
                foregroundColor = tileColor;
                backgroundColor = tileColor;
                final int height = tileArea.height;
                final Area netArea = new Area((width*5)/6, (height*5)/6);
                drawGridShape(center, netArea, oddFlag);
            }
            interior = interiorGridShape(center, tileArea, oddFlag);
        }

        final int markingCount = markings.size();
        int markingWidth = interior.width;
        int markingHeight = interior.height;
        if (markingCount == 2) {
            markingWidth /= 2;
       } else if (markingCount == 3 || markingCount == 4) {
            markingWidth /= 2;
            markingHeight /= 2;
        } else {
            assert markingCount == 1 : markingCount;
        }

        final Color markingColor = markings.getColor();

        for (int ind = 0; ind < markingCount; ind++) {
            int markingLeft = interior.x;
            int markingTop = interior.y;
            if (markingCount == 2) {
                markingLeft += ind * markingWidth;
            } else if (markingCount == 3 || markingCount == 4) {
                markingLeft += (ind%2) * markingWidth;
                markingTop += (ind/2) * markingHeight;
            } else {
                assert markingCount == 1 : markingCount;
            }

            final Rect markingBounds = new Rect(markingLeft,
                    markingTop, markingWidth, markingHeight);
            final AttrMode mode = markings.getMode(ind);
            final Attr marking = markings.getMarking(ind);
            drawMarking(markingBounds, mode, marking, markingColor);
        }

        return interior;
    }

    private static Shape getCellShape() {
        return Game.getInstance().getOpt().getGrid().getCellShape();
    }

    private static void initializeShapes() {
        assert shapes == null : shapes;
        shapes = new Poly[Attr.COUNT_MAX];

        // polygons for tile markings

        final Poly roundel = new Poly();
        for (int i = 0; i < 20; i++) {
            final double phi = Math.PI/10 * i;
            final double x = 0.5 + 0.44*Math.cos(phi);
            final double y = 0.5 + 0.44*Math.sin(phi);
            roundel.add(x, y);
        }
        shapes[0] = roundel;

        final Poly delf = new Poly();
        delf.add(0.15, 0.15);
        delf.add(0.85, 0.15);
        delf.add(0.85, 0.85);
        delf.add(0.15, 0.85);
        shapes[1] = delf;

        final Poly triangle = new Poly();
        triangle.add(0.05, 0.2);
        triangle.add(0.5, 0.95);
        triangle.add(0.95, 0.2);
        shapes[2] = triangle;

        final Poly starCross = new Poly();
        starCross.add(0.0, 0.5);
        starCross.add(0.4, 0.6);
        starCross.add(0.5, 1.0);
        starCross.add(0.6, 0.6);
        starCross.add(1.0, 0.5);
        starCross.add(0.6, 0.4);
        starCross.add(0.5, 0.0);
        starCross.add(0.4, 0.4);
        shapes[3] = starCross;

        final Poly mulletOfSeven = new Poly();
        for (int i = 0; i < 14; i++) {
            final double phi = Math.PI/7 * i;
            double x, y;
            if (Global.isOdd(i)) {
                x = 0.5 + 0.2*Math.sin(phi);
                y = 0.5 + 0.2*Math.cos(phi);
            } else {
                x = 0.5 * (1 + Math.sin(phi));
                y = 0.5 * (1 + Math.cos(phi));
            }
            mulletOfSeven.add(x, y);
        }
        shapes[4] = mulletOfSeven;

        final Poly lozenge = new Poly();
        lozenge.add(0.5, 0.0);
        lozenge.add(1.0, 0.5);
        lozenge.add(0.5, 1.0);
        lozenge.add(0.0, 0.5);
        shapes[5] = lozenge;

        final Poly heart = new Poly();
        heart.add(0.5, 0.0);

        heart.add(0.1, 0.45);
        heart.add(0.05, 0.65);
        heart.add(0.1, 0.8);
        heart.add(0.2, 0.9);
        heart.add(0.4, 0.9);

        heart.add(0.5, 0.75);

        heart.add(0.6, 0.9);
        heart.add(0.8, 0.9);
        heart.add(0.9, 0.8);
        heart.add(0.95, 0.65);
        heart.add(0.9, 0.45);

        shapes[6] = heart;

        final Poly trefoil = new Poly();
        trefoil.add(0.5, 1.0);

        trefoil.add(0.36, 0.94);
        trefoil.add(0.3, 0.8);
        trefoil.add(0.36, 0.66);
        trefoil.add(0.34, 0.64);
        trefoil.add(0.2, 0.7);
        trefoil.add(0.06, 0.64);
        trefoil.add(0.0, 0.5);
        trefoil.add(0.06, 0.36);
        trefoil.add(0.2, 0.3);
        trefoil.add(0.34, 0.36);
        trefoil.add(0.44, 0.34);
        trefoil.add(0.4, 0.1);
        trefoil.add(0.3, 0.0);

        trefoil.add(0.7, 0.0);
        trefoil.add(0.6, 0.1);
        trefoil.add(0.56, 0.34);
        trefoil.add(0.66, 0.36);
        trefoil.add(0.8, 0.3);
        trefoil.add(0.94, 0.36);
        trefoil.add(1.0, 0.5);
        trefoil.add(0.94, 0.64);
        trefoil.add(0.8, 0.7);
        trefoil.add(0.66, 0.64);
        trefoil.add(0.64, 0.66);
        trefoil.add(0.7, 0.8);
        trefoil.add(0.64, 0.94);

        shapes[7] = trefoil;

        final Poly spade = new Poly();
        spade.add(0.5, 1.0);

        spade.add(0.1, 0.6);
        spade.add(0.05, 0.44);
        spade.add(0.1, 0.28);
        spade.add(0.2, 0.2);
        spade.add(0.4, 0.2);
        spade.add(0.49, 0.3);
        spade.add(0.3, 0.0);

        spade.add(0.7, 0.0);
        spade.add(0.51, 0.3);
        spade.add(0.6, 0.2);
        spade.add(0.8, 0.2);
        spade.add(0.9, 0.28);
        spade.add(0.95, 0.44);
        spade.add(0.9, 0.6);

        shapes[8] = spade;
    }

    private static void initializeTargetArrow() {
        assert targetArrow == null : targetArrow;
        targetArrow = new Poly();

        // polygonal arrow symbol for indicating the target cell
        targetArrow.add(0.6, 0.4);
        targetArrow.add(0.5, 0.9);
        targetArrow.add(0.45, 0.65);
        targetArrow.add(0.2, 0.95);
        targetArrow.add(0.05, 0.8);
        targetArrow.add(0.35, 0.55);
        targetArrow.add(0.1, 0.5);
        targetArrow.add(0.6, 0.4);
    }

    public static Rect interiorGridShape(Point center, Area area,
            boolean oddFlag)
    {
        final Rect bounds = new Rect(center, area);

        switch (getCellShape()) {
            case HEXAGON:
                assert !oddFlag;
                return interiorHexagon(bounds);
            case SQUARE:
                assert area.isSquare();
                return bounds;
            case TRIANGLE:
                return interiorEquilateralTriangle(bounds, oddFlag);
            default:
                throw new AssertionError(getCellShape());
       }
    }
}
