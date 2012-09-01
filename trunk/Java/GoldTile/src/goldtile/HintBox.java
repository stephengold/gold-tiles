// File:     HintBox.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  HintBox class for the Gold Tile Game
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

import java.awt.BorderLayout;
import java.awt.Insets;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

public class HintBox
    extends JDialog
    implements java.awt.event.ActionListener
{
    // per-instance fields, sorted by type
    private HintStrength strength = null;
    final private JButton cancelButton = new JButton("Cancel");
    final private JButton okButton = new JButton("OK");
    final private JRadioButton none = new JRadioButton("none");
    final private JRadioButton unused = new JRadioButton("unused cells");
    final private JRadioButton connected = new JRadioButton(
            "unused cells connected to the start cell");
    final private JRadioButton usableByPlayable = new JRadioButton(
            "cells usable by playable tiles");
    final private JRadioButton usableByActive = new JRadioButton(
            "cells usable by the active tile");

    // constructors
    public HintBox(java.awt.Frame parent) {
        super(parent, true); // based on a modal JDialog

        setTitle("Hint Controls - Gold Tile Game");
        final Border border = new EmptyBorder(new Insets(5, 10, 5, 10));

        // question along the top edge

        final JLabel question = new JLabel(
                "Which cells would you like highlighted?");
        question.setBorder(border);
        add(question, BorderLayout.BEFORE_FIRST_LINE);

        // radio buttons in the center

        final ButtonGroup group = new ButtonGroup();
        final Box radio = new Box(BoxLayout.PAGE_AXIS);
        radio.setBorder(border);

        for (HintStrength value : HintStrength.values()) {
            final JRadioButton button = getButton(value);
            button.setActionCommand(value.name());
            button.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            group.add(button);
            radio.add(button);
        }
        add(radio, BorderLayout.CENTER);

        // row of buttons along the bottom edge

        okButton.setPreferredSize(new java.awt.Dimension(200, 0)); // wide button

        final Box buttonRow = new Box(BoxLayout.LINE_AXIS);
        buttonRow.setBorder(border);

        buttonRow.add(cancelButton);
        buttonRow.add(Box.createHorizontalStrut(20));
        buttonRow.add(Box.createHorizontalGlue());
        buttonRow.add(okButton);
        add(buttonRow, BorderLayout.AFTER_LAST_LINE);
    }

    // methods, sorted by name

    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;

        final String command = event.getActionCommand();
        switch (command) {
            case "Cancel":
                strength = null;
                dispose();
                break;
            case "OK":
                dispose();
                break;
            default:
                strength = HintStrength.valueOf(command);
        }
    }

    private JRadioButton getButton(HintStrength value) {
        switch (value) {
            case NONE:
                return none;
            case UNUSED:
                return unused;
            case CONNECTED:
                return connected;
            case USABLE_BY_PLAYABLE:
                return usableByPlayable;
            case USABLE_BY_ACTIVE:
                return usableByActive;
            default:
                throw new AssertionError(value);
        }
    }

    public HintStrength run(HintStrength old, GameStyle style) {
        assert old != null;
        assert style != null;

        this.strength = old;
        JRadioButton button = getButton(old);
        button.setSelected(true);

        cancelButton.addActionListener(this);
        okButton.addActionListener(this);
        for (HintStrength value : HintStrength.values()) {
            button = getButton(value);
            button.addActionListener(this);
        }

        // limit choices for challenge games
        final boolean allow = style.allowsHints();
        connected.setEnabled(allow);
        usableByPlayable.setEnabled(allow);
        usableByActive.setEnabled(allow);

        pack();
        setLocationRelativeTo(getParent());
        setVisible(true);

        return this.strength;
    }
}
