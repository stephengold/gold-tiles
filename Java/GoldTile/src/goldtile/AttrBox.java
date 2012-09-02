// File:     AttrBox.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  AttrBox class for the Gold Tile Game
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

public class AttrBox
    extends JDialog
    implements java.awt.event.ActionListener,
       javax.swing.event.ListSelectionListener
{
     final private static int MODE_COUNT = AttrMode.values().length;
     final private static Border BORDER =
             new EmptyBorder(new Insets(5, 10, 5, 10));

     // embedded classes

    private static class Column extends Box {
        final private JList list;

        public Column(AttrBox parent, int iAttr) {
            super(BoxLayout.PAGE_AXIS);

            final String text = String.format("%s attribute:",
                    StringExt.ordinal(iAttr + 1));
            final JLabel label = new JLabel(text);
            label.setAlignmentX(JLabel.CENTER_ALIGNMENT);

            list = new JList();
            list.setAlignmentX(JList.CENTER_ALIGNMENT);
            list.setLayoutOrientation(JList.VERTICAL);
            list.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
            list.setListData(getModesData());

            final ListSelectionModel selectionModel = list.getSelectionModel();
            selectionModel.addListSelectionListener(parent);

            setBorder(BORDER);
            add(label);
            add(list);
        }

        public AttrMode getValue() {
            final int iMode = list.getSelectedIndex();
            if (iMode == -1) {
                return null;
            }
            return getMode(iMode);
        }

        public void setValue(AttrMode mode) {
            for (int iMode = 0; iMode < MODE_COUNT; iMode++) {
                if (getMode(iMode).equals(mode)) {
                    list.setSelectedIndex(iMode);
                    return;
                }
            }
            throw new AssertionError(mode);
        }
    }

    // per-instance fields, sorted by type
    final private Column columns[] = new Column[GameOpt.ATTR_COUNT_MAX];
    private DisplayModes displayModes = null; // the "model"
    final private JButton cancelButton = new JButton("Cancel");
    final private JButton okButton = new WideButton("OK");

    // constructors

    public AttrBox(java.awt.Frame parent) {
        super(parent, true); // based on a modal JDialog

        setTitle("Attribute Display - Gold Tile Game");

        final Box attrRow = new Box(BoxLayout.LINE_AXIS);
        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            final Column column = new Column(this, iAttr);
            attrRow.add(column);
            columns[iAttr] = column;
        }

       // row of buttons along the bottom edge
        final Box buttonRow = new Box(BoxLayout.LINE_AXIS);
        buttonRow.setBorder(BORDER);
        buttonRow.add(cancelButton);
        buttonRow.add(Box.createHorizontalStrut(20));
        buttonRow.add(Box.createHorizontalGlue());
        buttonRow.add(okButton);

        add(attrRow, BorderLayout.CENTER);
        add(buttonRow, BorderLayout.AFTER_LAST_LINE);
    }

    // methods, sorted by name

    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;

        final String command = event.getActionCommand();
        switch (command) {
            case "Cancel":
                displayModes = null;
                dispose();
                break;
            case "OK":
                dispose();
                break;
            default:
                throw new AssertionError(command);
        }
    }

    private static String[] getModesData() {
        String[] result = new String[MODE_COUNT];

        for (int iMode = 0; iMode < MODE_COUNT; iMode++) {
            final AttrMode mode = getMode(iMode);
            result[iMode] = mode.getDescription();
        }

        return result;
    }

    private static AttrMode getMode(int iMode) {
        switch (iMode) {
            case 0:
                return AttrMode.ABC;
            case 1:
                return AttrMode.RST;
            case 2:
                return AttrMode.NUMERAL;
            case 3:
                return AttrMode.COLOR;
            case 4:
                return AttrMode.SHAPE;
            default:
                throw new AssertionError(iMode);
        }
    }

    public DisplayModes run(DisplayModes old) {
        assert old != null;

        displayModes = new DisplayModes(old);

        cancelButton.addActionListener(this);
        okButton.addActionListener(this);

        updateView();
        pack();
        setLocationRelativeTo(getParent());
        setVisible(true);

        return displayModes;
    }

    /**
     * Process an event indicating that a JList changed.
     * @param event
     */
    @Override
    public void valueChanged(javax.swing.event.ListSelectionEvent event) {
        if (event.getValueIsAdjusting()) {
            return;
        }
        updateModel();
    }

    public void updateModel() {
        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            final AttrMode value = columns[iAttr].getValue();
            if (value != null) {
                displayModes.setMode(iAttr, value);
            }
        }
        // TODO make consistent!
    }

    public void updateView() {
        final int attrCount = Game.getInstance().getOpt().getAttrCount();
        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            columns[iAttr].setValue(displayModes.getMode(iAttr));
            columns[iAttr].setVisible(iAttr < attrCount);
        }
    }
}
