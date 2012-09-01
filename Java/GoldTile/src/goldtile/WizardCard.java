// File:     WizardCard.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  WizardCard class for the Gold Tile Game
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

public abstract class WizardCard
    extends javax.swing.JPanel
{
    // per-instance data
    final protected Wizard wizard;

    // constructors

    public WizardCard(Wizard wizard, String name) {
        super(); // default layout manager

        assert wizard != null;
        assert name != null;

        this.wizard = wizard;
        setName(name);
    }

    // methods, sorted by name

    public void backAction() {
        // sometimes there's no back button
    }

    public String getTitle() {
        return "TODO";
    }

    abstract public void nextAction();

    public void setModels(Object[] models) {
    }

    final protected void showCard(String cardName, Object... parameters) {
        wizard.showCard(cardName, parameters);
    }
}
