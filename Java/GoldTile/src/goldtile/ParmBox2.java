// File:     ParmBox2.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ParmBox2 class for the Gold Tile Game
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

import java.awt.event.ActionListener;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

public class ParmBox2
    extends WizardCard
    implements ActionListener // radio buttons and Dim fields
{
    // constants
    final private static int GRID_BUTTON_COUNT = 4;
    final private static int TOPOLOGY_BUTTON_COUNT = 7;
    
    // embedded classes, sorted by name
    
    private class DimField extends JTextField {
        DimField(ActionListener listener) {
            super(Dim.endlessString(), 10); // 10 characters wide
            setHorizontalAlignment(JTextField.RIGHT);
            
            // change font to match JLabel
            final java.awt.Font labelFont = new JLabel().getFont();
            setFont(labelFont);
            
            addActionListener(listener);
            setInputVerifier(new DimVerifier());
            
            setMaximumSize(new java.awt.Dimension(70, 30));
        }
        
        public Dim getValue() {
            final String text = getText();
            return new Dim(text);
        }
        
        public void setValue(Dim value) {
            final String text = value.toString();
            setText(text);
        }
    }
    
    public class GridIcon implements Icon {
        // methods, sorted by name
        @Override
        public int getIconHeight() { return 90; }
        @Override
        public int getIconWidth() { return 90; }
        @Override
        public void paintIcon(java.awt.Component component, 
                java.awt.Graphics graphics, int x, int y)
        {
            // TODO
        }
    }
    
    private static class InvalidGridException extends Exception {}
    private static class InvalidTopologyException extends Exception {}
    
    // per-instance fields, sorted by type
    final private DimField heightField;
    final private DimField widthField;
    private GameOpt gameOpt = null;
    private HandOpt[] handOpts = null;
    final private JLabel cellsLabel = new JLabel("TODO cells");
    final private JRadioButton gridButtons[] = 
            new JRadioButton[GRID_BUTTON_COUNT];
    final private JRadioButton topologyButtons[] = 
            new JRadioButton[TOPOLOGY_BUTTON_COUNT];

    // constructors
    
    @SuppressWarnings("LeakingThisInConstructor")
    public ParmBox2(Wizard wizard) {
        super(wizard, ParmBox2.class.getName());
        
        // board types
        
        final Box topologyBoxLeft = new Box(BoxLayout.PAGE_AXIS);
        topologyBoxLeft.setAlignmentY(Box.TOP_ALIGNMENT);
        final Box topologyBoxRight = new Box(BoxLayout.PAGE_AXIS);
        topologyBoxRight.setAlignmentY(Box.TOP_ALIGNMENT);
        final ButtonGroup topologyGroup = new ButtonGroup(); 
        
        for (Topology topology : Topology.values()) {
            try {
                final int iButton = getButtonIndex(topology);
                final JRadioButton button = new JRadioButton();
                button.setText(getButtonText(topology));
                button.setActionCommand(topology.name());
                button.addActionListener(this);
                button.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            
                if (iButton < 3) {
                    topologyBoxLeft.add(button);
                    if (iButton < 2) {
                        topologyBoxLeft.add(Box.createVerticalGlue());
                    }
                } else {
                    topologyBoxRight.add(button);
                }
                topologyGroup.add(button);
                topologyButtons[iButton] = button;
            } catch (InvalidTopologyException exception) {
            }
        }
        
        final Box topologyBox = new Box(BoxLayout.LINE_AXIS);
        final Border topologyBorder = BorderFactory.createTitledBorder( 
                BorderFactory.createEtchedBorder(EtchedBorder.LOWERED),
                "Which sort of board would you prefer?" );
        topologyBox.setBorder(topologyBorder);
        topologyBox.add(topologyBoxLeft);
        topologyBox.add(Box.createHorizontalGlue());
        topologyBox.add(topologyBoxRight);
        
        // grid types
        
        final Box gridBox = new Box(BoxLayout.PAGE_AXIS);
        final ButtonGroup gridGroup = new ButtonGroup();
        
        for (Grid grid : Grid.values()) {
            try {
                final int iButton = getButtonIndex(grid);
                final JRadioButton button = new JRadioButton();
                button.setText(getButtonText(grid));
                button.setActionCommand(grid.name());
                button.addActionListener(this);
                button.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            
                gridBox.add(button);
                gridGroup.add(button);
                gridButtons[iButton] = button;
            } catch (InvalidGridException exception) {
            }
        }
        
        final Box lowerLeft = new Box(BoxLayout.LINE_AXIS);
        final Border gridBorder = BorderFactory.createTitledBorder( 
                BorderFactory.createEtchedBorder(EtchedBorder.LOWERED),
                "Which sort of grid would you prefer?" );
        lowerLeft.setBorder(gridBorder);
        lowerLeft.add(gridBox);
        final JLabel gridImage = new JLabel(new GridIcon());
        lowerLeft.add(gridImage);        
        
        // board width/height textfields with custom verifiers

        final Box widthBox = new Box(BoxLayout.LINE_AXIS);
        widthBox.setAlignmentX(JRadioButton.CENTER_ALIGNMENT);
        widthBox.add(new JLabel("Board width: "));
        widthField = new DimField(this);
        widthBox.add(widthField);
        widthBox.add(new JLabel(" columns"));
        
        final Box heightBox = new Box(BoxLayout.LINE_AXIS);
        heightBox.setAlignmentX(JRadioButton.CENTER_ALIGNMENT);
        heightBox.add(new JLabel("Board height: "));
        heightField = new DimField(this);
        heightBox.add(heightField);
        heightBox.add(new JLabel(" rows"));
        
        cellsLabel.setAlignmentX(JRadioButton.CENTER_ALIGNMENT);

        // put it all together
        
        final Box bottomRight = new Box(BoxLayout.PAGE_AXIS);
        bottomRight.add(Box.createVerticalGlue());
        bottomRight.add(widthBox);
        bottomRight.add(heightBox);
        bottomRight.add(Box.createVerticalGlue());
        bottomRight.add(cellsLabel);
        bottomRight.add(Box.createVerticalGlue());
        
        final Box bottom = new Box(BoxLayout.LINE_AXIS);
        bottom.add(lowerLeft);
        bottom.add(bottomRight);
        
        final Box mainBox = new Box(BoxLayout.PAGE_AXIS);
        mainBox.add(topologyBox);
        mainBox.add(bottom);
        
        add(mainBox);
    }
    
    // methods, sorted by name

    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;
        
        updateModel();

        final String command = event.getActionCommand();        
        try {
            final Grid grid = Grid.valueOf(command);
            
            // from a grid button        
            gameOpt.setGrid(grid);
            
        } catch (IllegalArgumentException exception) {
        }
        
        try {
            final Topology topology = Topology.valueOf(command);

            // from a topology button        
            gameOpt.setBoardWrap(topology.doesWrap());
            
            if (topology.hasFiniteHeight() != gameOpt.hasFiniteHeight()) {
                if (topology.hasFiniteHeight()) {
                    gameOpt.setBoardHeight(GameOpt.BOARD_HEIGHT_DEFAULT);
                } else {
                    gameOpt.setBoardHeight(Dim.endless());
                }
            }

            if (topology.hasFiniteWidth() != gameOpt.hasFiniteWidth()) {
                if (topology.hasFiniteWidth()) {
                    gameOpt.setBoardWidth(GameOpt.BOARD_WIDTH_DEFAULT);
                } else {
                    gameOpt.setBoardWidth(Dim.endless());
                }
            }

        } catch (IllegalArgumentException exception) {
        }
 
        updateView();
    }
    
    @Override
    public void backAction() {
        updateModel();
        showCard(ParmBox1.class.getName(), gameOpt, handOpts);            
    }

    private int getButtonIndex(Grid grid) 
            throws InvalidGridException
    {
        switch (grid) {
            case GRID_4WAY:
                return 0;
            case GRID_8WAY:
                return 1;
            case GRID_HEX:
                return 2;
            case GRID_TRIANGLE:
                return 3;
            default:
                throw new InvalidGridException();
        }
    }

    private int getButtonIndex(Topology topology) 
            throws InvalidTopologyException
    {
        switch (topology) {
            case FLAT:
                return 0;
            case RECTANGLE:
                return 1;
            case TORUS:
                return 2;
            case VSTRIP:
                return 3;
            case HSTRIP:
                return 4;
            case VCYLINDER:
                return 5;
            case HCYLINDER:
                return 6;
            default:
                throw new InvalidTopologyException();
        }
    }

    private String getButtonText(Grid grid) {
        switch (grid) {
            case GRID_4WAY:
                return "Square grid (4-way)";
            case GRID_8WAY:
                return "Square grid (8-way)";
            case GRID_HEX:
                return "Hexagonal grid";
            case GRID_TRIANGLE:
                return "Triangular grid";
            default:
                throw new AssertionError(grid);
        }
    }
    
    private String getButtonText(Topology topology) {
        switch (topology) {
            case FLAT:
                return "Flat and endless (no wrap)";
            case RECTANGLE:
                return "Rectangle/square (4 edges)";
            case TORUS:
                return "Torus (wraps in all directions)";
            case VSTRIP:
                return "Vertical strip (left/right edges)";
            case HSTRIP:
                return "Horizontal strip (top/bottom edges)";
            case VCYLINDER:
                return "Vertical cylinder (wraps left/right)";
            case HCYLINDER:
                return "Horizontal cylinder (wraps top/bottom)";
            default:
                throw new AssertionError(topology);
        }
    }
    
    @Override 
    public String getTitle() {
        return "Board Parameters";
    }
    
    @Override
    public void nextAction() {
        updateModel();
        
        showCard(ParmBox3.class.getName(), gameOpt, handOpts);
    }
    
    @Override
    public void setModels(Object[] models) {
        assert models != null;
        assert models.length == 2 : models.length;
        assert models[0] != null;
        assert models[1] != null;
        
        gameOpt = (GameOpt)models[0];
        gameOpt.validate();
        handOpts = (HandOpt[])models[1];
        
        updateView();
    }
    
    private void updateModel() {
        final Dim height = heightField.getValue();
        gameOpt.setBoardHeight(height);

        final Dim width = widthField.getValue();
        gameOpt.setBoardWidth(width);

        gameOpt.validate();
    }
    
    public void updateView() {
        // the wizard's back/next buttons
        wizard.getBackButton().setText("\u21d0 Back");
        wizard.getBackButton().setEnabled(true);
        wizard.getBackButton().setVisible(true);
        wizard.getNextButton().setText("Next \u21d2");
        wizard.getNextButton().setEnabled(true);
        
        // button values
        
        final Grid grid = gameOpt.getGrid();
        try {
            final int iButton = getButtonIndex(grid);
            gridButtons[iButton].setSelected(true);    
        } catch (InvalidGridException exception) {
            throw new AssertionError();
        }
        
        final Topology topology = gameOpt.getTopology();
        try {
            final int iButton = getButtonIndex(topology);
            topologyButtons[iButton].setSelected(true);    
        } catch (InvalidTopologyException exception) {
            throw new AssertionError();
        }
        
        // dimensions
        final Dim height = gameOpt.getBoardHeight();
        heightField.setValue(height);
        final Dim width = gameOpt.getBoardWidth();
        widthField.setValue(width);
        
        // cell count
        final long cellCount = gameOpt.getCellCount();
        String text = Long.toString(cellCount);
        if (cellCount == Long.MAX_VALUE) {
            text = Dim.endlessString();
        }
        cellsLabel.setText(String.format("\u21d2 The board has %s cells.", 
                    text));            
    }    
}
