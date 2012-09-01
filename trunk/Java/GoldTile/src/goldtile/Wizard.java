// File:     Wizard.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  Wizard class for the Gold Tile Game
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
import java.awt.CardLayout;
import java.awt.Insets;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

public class Wizard
    extends JDialog
    implements java.awt.event.ActionListener
{
    // per-instance fields, sorted by type
    private boolean complete = false;
    final private CardLayout cardLayout = new CardLayout();
    final private JButton backButton = new JButton("< Back");
    final private JButton nextButton = new JButton("Next >");
    final private JButton cancelButton = new JButton("Cancel");
    final private JPanel cardPanel = new JPanel();
    private WizardCard visibleCard = null;
    final private java.util.Map<String,WizardCard> map =
            new java.util.TreeMap<>();

    // constructors

    public Wizard(java.awt.Frame parent) {
        super(parent, true); // based on a modal JDialog

        final Border wizardBorder = new EmptyBorder(new Insets(5, 10, 5, 10));

        // row of buttons along the bottom edge

        backButton.setActionCommand("back");
        nextButton.setPreferredSize(new java.awt.Dimension(200, 0)); // wide button
        nextButton.setActionCommand("next");

        final Box buttonRow = new Box(BoxLayout.LINE_AXIS);
        buttonRow.setBorder(wizardBorder);

        buttonRow.add(cancelButton);
        buttonRow.add(Box.createHorizontalStrut(20));
        buttonRow.add(Box.createHorizontalGlue());
        buttonRow.add(backButton);
        buttonRow.add(Box.createHorizontalStrut(10));
        buttonRow.add(nextButton);

        // cards in the center

        cardPanel.setBorder(wizardBorder);
        cardPanel.setLayout(cardLayout);

        add(buttonRow, BorderLayout.PAGE_END);
        add(cardPanel, BorderLayout.CENTER);
    }

    // methods, sorted by name

    final public void addCard(WizardCard card) {
        assert card != null;

        final String name = card.getName();
        assert !map.containsKey(name);

        cardPanel.add(card, name);
        map.put(name, card);
    }

    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;

        final String command = event.getActionCommand();

        switch (command) {
            case "Cancel":
                dispose();
                break;
            case "back":
                visibleCard.backAction();
                break;
            case "next":
                visibleCard.nextAction();
                break;
            default:
                throw new AssertionError(command);
        }
    }

    public void complete() {
        this.complete = true;
        dispose();
    }

    public AbstractButton getBackButton() {
        return backButton;
    }

    public AbstractButton getNextButton() {
        return nextButton;
    }

    public boolean run(WizardCard firstCard, Object... parameters) {
        cancelButton.addActionListener(this);
        backButton.addActionListener(this);
        nextButton.addActionListener(this);

        complete = false;
        showCard(firstCard, parameters);
        pack();
        setLocationRelativeTo(getParent());
        setVisible(true);

        return complete;
    }

    public void showCard(String cardName, Object... models) {
        assert cardName != null;

        final WizardCard card = map.get(cardName);
        assert card != null;

        showCard(card, models);
    }

    private void showCard(WizardCard card, Object... models) {
        assert card != null;

        visibleCard = card;
        card.setModels(models);
        setTitle(card.getTitle() + " - Gold Tile Game");
        cardLayout.show(cardPanel, card.getName());
    }
}
