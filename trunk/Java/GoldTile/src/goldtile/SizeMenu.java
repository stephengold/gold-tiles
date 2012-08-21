// File:     SizeMenu.java
// Location: Java
// Purpose:  SizeMenu class for the Gold Tile Game
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

import java.awt.event.KeyEvent;
import javax.swing.ButtonGroup;
import javax.swing.JMenuItem;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.KeyStroke;

public class SizeMenu 
    extends javax.swing.JMenu 
    implements java.awt.event.ActionListener
{
    // constants
    public final static int COUNT = 7;
    public final static int MAX = COUNT - 1;
    public final static int MIN = 0;

    public MenuBar menuBar = null;
    private int value = 0;
    private final JMenuItem zoomIn = new JMenuItem("Zoom In");
    private final JMenuItem zoomOut = new JMenuItem("Zoom Out");
    private final JRadioButtonMenuItem sizeItems[] 
            = new JRadioButtonMenuItem[COUNT];  
    
    @SuppressWarnings("LeakingThisInConstructor")
    SizeMenu(String label, int value) {
        setText(label);
        
        zoomOut.addActionListener(this);
        zoomOut.setActionCommand("shrink");
        zoomOut.setMnemonic(KeyEvent.VK_Z);
        add(zoomOut);
        
        addSeparator();
        
        sizeItems[0] = new JRadioButtonMenuItem("1 Tiny");
        sizeItems[0].setActionCommand("0");
        sizeItems[0].setMnemonic(KeyEvent.VK_1);
        
        sizeItems[1] = new JRadioButtonMenuItem("2 Small");
        sizeItems[1].setActionCommand("1");
        sizeItems[1].setMnemonic(KeyEvent.VK_2);
        
        sizeItems[2] = new JRadioButtonMenuItem("3 Smallish");
        sizeItems[2].setActionCommand("2");
        sizeItems[2].setMnemonic(KeyEvent.VK_3);
        
        sizeItems[3] = new JRadioButtonMenuItem("4 Medium");
        sizeItems[3].setActionCommand("3");
        sizeItems[3].setMnemonic(KeyEvent.VK_4);
        
        sizeItems[4] = new JRadioButtonMenuItem("5 Largish");
        sizeItems[4].setActionCommand("4");
        sizeItems[4].setMnemonic(KeyEvent.VK_5);
        
        sizeItems[5] = new JRadioButtonMenuItem("6 Large");
        sizeItems[5].setActionCommand("5");
        sizeItems[5].setMnemonic(KeyEvent.VK_6);
        
        sizeItems[6] = new JRadioButtonMenuItem("7 Very Large");
        sizeItems[6].setActionCommand("6");
        sizeItems[6].setMnemonic(KeyEvent.VK_7);
        
        final ButtonGroup group = new ButtonGroup();
        for (JRadioButtonMenuItem item : sizeItems) {
            item.addActionListener(this);
            group.add(item);
            add(item);
        }

        addSeparator();
        
        zoomIn.setActionCommand("enlarge");
        zoomIn.addActionListener(this);
        add(zoomIn);
        
        setValue(value);
    }

    // methods
    
    public void accelerate() {
        zoomIn.setAccelerator(KeyStroke.getKeyStroke('+'));
        zoomOut.setAccelerator(KeyStroke.getKeyStroke('-'));
    }
    
    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        final String command = event.getActionCommand();
        switch (command) {
            case "enlarge": 
                if (value < MAX) {
                    setValue(value + 1);
                }
                break;
                
            case "shrink": 
                if (value > MIN) {
                    setValue(value - 1);
                }
                break;
            
            case "0":
            case "1":
            case "2":
            case "3":
            case "4":
            case "5":
            case "6":
                final Integer size = Integer.parseInt(command);
                setValue(size.intValue());
                break;

            default:
                throw new AssertionError(command);
        }
    }
    
    public int getValue() {
        return value;
    }
    
    final public void setValue(int value) {
        assert value <= MAX : value;
        assert value >= MIN : value;

        final JRadioButtonMenuItem item = sizeItems[value];
        item.setSelected(true);
        this.value = value;
        
        if (menuBar != null) {
            menuBar.clientArea.repaint();
        }
    }
}
