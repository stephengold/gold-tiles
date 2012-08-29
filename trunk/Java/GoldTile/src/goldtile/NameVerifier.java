// File:     NameVerifier.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  NameVerifier class for the Gold Tile Game
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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JComponent;
import javax.swing.JTextField;

public class NameVerifier
    extends javax.swing.InputVerifier
{
    // check input (may alter the text or block transfer of focus)
    // return true if focus may be transfered, false to block transfer
    @Override
    public boolean shouldYieldFocus(JComponent input) {
        assert input != null;
        
        final JTextField textField = (JTextField)input;
        String text = textField.getText();
        text = StringExt.normalizeName(text);
        if (text.equals("") || text.equals("Computer")) {
            return false;
        }
        
        textField.setText(text);
        assert verify(input);
        
        // Fire a new action event to all listeners.
        final int eventId = ActionEvent.ACTION_LAST + 1;
        final ActionEvent event = new ActionEvent(input, eventId, "yieldFocus");
        final ActionListener[] listeners = textField.getActionListeners();
        for (ActionListener listener : listeners) {
            listener.actionPerformed(event);
        }
        
        return true;
    }
         
    // check input (with no side effects)
    @Override
    public boolean verify(JComponent input) {
        assert input != null;
        
        final JTextField textField = (JTextField)input;
        final String text = textField.getText();
        final String normal = StringExt.normalizeName(text);
        
        return !normal.equals("") && !normal.equals("Computer");
    }
}
