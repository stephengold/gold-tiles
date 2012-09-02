// File:     HandBox.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  HandBox class for the Gold Tile Game
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

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

public class HandBox
    extends WizardCard
    implements java.awt.event.ActionListener // radio buttons and combo boxes
{
    // nested classes

    private static class OptBox
        extends Box
    {
        final private Box ipRow = new Box(BoxLayout.LINE_AXIS);
        final private Box levelRow = new Box(BoxLayout.LINE_AXIS);
        final private Box nameRow = new Box(BoxLayout.LINE_AXIS);
        final private JRadioButton autoButton = new JRadioButton();
        final private JRadioButton localButton = new JRadioButton();
        final private JRadioButton remoteButton = new JRadioButton();
        final private JSlider levelSlider = new JSlider();
        final private JTextField ipField = new JTextField();
        final private JTextField nameField = new JTextField();

        private OptBox(HandBox parent, int iHand) {
            super(BoxLayout.PAGE_AXIS);

            assert parent != null;
            assert iHand >= 0;
            assert iHand < ParmBox3.HANDS_DEALT_MAX;

            final String text = String.format("Who will play the %s hand?",
                    StringExt.ordinal(iHand + 1));
            final Border styleBorder = BorderFactory.createTitledBorder(
                    BorderFactory.createEtchedBorder(EtchedBorder.LOWERED),
                    text);
            setBorder(styleBorder);

            // radio buttons

            final ButtonGroup group = new ButtonGroup();

            localButton.setText("someone at this computer");
            localButton.addActionListener(parent);
            localButton.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            group.add(localButton);

            autoButton.setText("this computer (automatic play)");
            autoButton.addActionListener(parent);
            autoButton.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            group.add(autoButton);

            remoteButton.setText("someone at another computer (network play)");
            remoteButton.addActionListener(parent);
            remoteButton.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
            group.add(remoteButton);

            // text field for player name

            final java.awt.Font labelFont = new JLabel().getFont();

            nameField.setFont(labelFont);
            nameField.setInputVerifier(new NameVerifier());
            nameField.setMaximumSize(new java.awt.Dimension(200, 30));

            nameRow.setAlignmentX(Box.LEFT_ALIGNMENT);
            nameRow.add(new JLabel("Player name: "));
            nameRow.add(nameField);

            // slider for "level of play"

            levelSlider.setMajorTickSpacing(1);
            levelSlider.setMaximum(HandOpt.LEVEL_MAX);
            levelSlider.setMinimum(HandOpt.LEVEL_MIN);
            levelSlider.setPaintTicks(true);
            levelSlider.setValue(HandOpt.LEVEL_DEFAULT);

            levelRow.setAlignmentX(Box.LEFT_ALIGNMENT);
            levelRow.add(new JLabel("Level of play:     easy "));
            levelRow.add(levelSlider);
            levelRow.add(new JLabel(" hard"));

            // text field for IP address

            ipField.setFont(labelFont);
            ipField.setInputVerifier(new AddressVerifier());
            ipField.setMaximumSize(new java.awt.Dimension(200, 30));

            ipRow.setAlignmentX(Box.LEFT_ALIGNMENT);
            ipRow.add(new JLabel("Player network address: "));
            ipRow.add(ipField);

            // stack up the components

            add(localButton);
            add(autoButton);
            add(remoteButton);
            add(nameRow);
            add(levelRow);
            add(ipRow);
        }

        public void updateModel(HandOpt model) {
            if (autoButton.isSelected()) {
                model.setAutomatic();
            } else if (remoteButton.isSelected()) {
                model.setRemote();
            } else {
                model.setLocalUser();
            }

            final String name = nameField.getText();
            if (name != null) {
                model.setPlayerName(name);
            }

            final String ip = ipField.getText();
            if (ip != null) {
                model.setIpAddress(ip);
            }

            final int level = levelSlider.getValue();
            model.setLevel(level);

            assert model.isValid();
        }

        public void updateView(ReadHandOpt model) {
            assert model.isValid();

            if (model.isAutomatic()) {
                autoButton.setSelected(true);
            } else if (model.isRemote()) {
                remoteButton.setSelected(true);
            } else {
                assert model.isLocalUser();
                localButton.setSelected(true);
            }

            ipField.setText(model.getIpAddress());
            nameField.setText(model.getSavedName());

            ipRow.setVisible(model.isRemote());
            levelRow.setVisible(model.isAutomatic());
            nameRow.setVisible(!model.isAutomatic());
        }
    }

    // per-instance fields, sorted by type
    private GameOpt gameOpt = null;
    private HandOpt[] handOpts = null;
    final private OptBox[] optBoxes = new OptBox[ParmBox3.HANDS_DEALT_MAX];

    // constructors

    @SuppressWarnings("LeakingThisInConstructor")
    public HandBox(Wizard wizard) {
        super(wizard, HandBox.class.getName());

        final Box hands = new Box(BoxLayout.PAGE_AXIS);

        for (int iHand = 0; iHand < ParmBox3.HANDS_DEALT_MAX; iHand++) {
            optBoxes[iHand] = new OptBox(this, iHand);
            hands.add(optBoxes[iHand]);
            hands.add(Box.createVerticalStrut(8));
        }
        hands.add(Box.createVerticalGlue());

        final JScrollPane scrollPane = new JScrollPane(hands);
        scrollPane.setBorder(null);
        scrollPane.setPreferredSize(new java.awt.Dimension(460, 240));
        add(scrollPane);
    }

    // methods, sorted by name

    /**
     * Process an event indicating that a radio button changed.
     * @param event
     */
    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;

        updateModel();
        updateView();
    }

    @Override
    public void backAction() {
        updateModel();
        if (gameOpt.getRules().isCustom()) {
            showCard(ParmBox3.class.getName(), gameOpt, handOpts);
        } else {
            showCard(ParmBox1.class.getName(), gameOpt, handOpts);
        }
    }

    @Override
    public String getTitle() {
        return "Hand Information";
    }

    @Override
    public void nextAction() {
        updateModel();
        wizard.complete();
    }

    @Override
    public void setModels(Object[] models) {
        assert models != null;
        assert models.length == 2 : models;
        assert models[0] != null;
        assert models[0] instanceof GameOpt;
        assert models[1] != null;
        assert models[1] instanceof HandOpt[];

        gameOpt = (GameOpt)models[0];
        gameOpt.validate();
        handOpts = (HandOpt[])models[1];
        for (int iHand = 0; iHand < ParmBox3.HANDS_DEALT_MAX; iHand++) {
            assert handOpts[iHand].isValid();
        }

        updateView();
    }

    private void updateModel() {
        for (int iHand = 0; iHand < ParmBox3.HANDS_DEALT_MAX; iHand++) {
            optBoxes[iHand].updateModel(handOpts[iHand]);
        }
    }

    private void updateView() {
        // the wizard's back/next buttons
        wizard.getBackButton().setText("\u21d0 Back");
        wizard.getBackButton().setEnabled(true);
        wizard.getBackButton().setVisible(true);
        wizard.getNextButton().setText("Play Game");
        wizard.getNextButton().setEnabled(true);

        for (int iHand = 0; iHand < ParmBox3.HANDS_DEALT_MAX; iHand++) {
            optBoxes[iHand].setVisible(iHand < gameOpt.getHandsDealt());
            optBoxes[iHand].updateView(handOpts[iHand]);
        }
    }
}
