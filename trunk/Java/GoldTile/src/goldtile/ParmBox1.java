// File:     ParmBox1.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ParmBox1 class for the Gold Tile Game
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

import java.awt.Dimension;
import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

public class ParmBox1
    extends WizardCard
    implements java.awt.event.ActionListener, // radio buttons
        javax.swing.event.ChangeListener,     // spinners
        java.awt.event.ItemListener           // checkbox
{
    // constants, sorted by type
    final private static Dimension SMALL = new Dimension(45, 20);
    final private static int STYLE_BUTTON_COUNT = 4;

    // embedded classes
    private static class InvalidStyleException extends Exception {}

    // per-instance fields, sorted by type
    private GameOpt gameOpt = null;
    private HandOpt[] handOpts = null;
    final private JCheckBox randomizeBox = new JCheckBox("randomized");
    final private JLabel challengeLabel = new JLabel(" minutes per hand");
    final private JLabel debugLabel =
            new JLabel("  OR   generated from seed: ");
    final private JList rulesList;
    final private JRadioButton[] styleButtons =
            new JRadioButton[STYLE_BUTTON_COUNT];
    final private JSpinner seedSpinner;
    final private JSpinner minutesSpinner;
    final private SpinnerNumberModel minutesModel;
    final private SpinnerNumberModel seedModel;

    // constructors

    @SuppressWarnings("LeakingThisInConstructor")
    public ParmBox1(Wizard wizard) {
        super(wizard, ParmBox1.class.getName());

        // Game styles and associated options

        final ButtonGroup styleGroup = new ButtonGroup();
        for (GameStyle style : GameStyle.values()) {
            try {
                final int iButton = getButtonIndex(style);
                final JRadioButton button = new JRadioButton();
                button.setText(getButtonText(style));
                button.setActionCommand(style.name());
                button.addActionListener(this);
                button.setAlignmentX(JRadioButton.LEFT_ALIGNMENT);
                styleGroup.add(button);
                styleButtons[iButton] = button;
            } catch (InvalidStyleException exception) {
            }
        }

        randomizeBox.addItemListener(this);

        final Number step = 1L;
        seedModel = new SpinnerNumberModel(GameOpt.SEED_DEFAULT,
                null, null, step);
        seedSpinner = new JSpinner(seedModel);
        seedSpinner.setEditor(new JSpinner.NumberEditor(seedSpinner, "#"));
        seedSpinner.addChangeListener(this);

        final Box debugOpt = new Box(BoxLayout.LINE_AXIS);
        debugOpt.setAlignmentX(Box.LEFT_ALIGNMENT);
        debugOpt.add(Box.createHorizontalStrut(30));
        debugOpt.add(randomizeBox);
        debugOpt.add(debugLabel);
        debugOpt.add(seedSpinner);

        final int max = GameOpt.MINUTES_PER_HAND_MAX;
        final int min = GameOpt.MINUTES_PER_HAND_MIN;
        final int value = GameOpt.MINUTES_PER_HAND_DEFAULT;
        minutesModel = new SpinnerNumberModel(value, min, max, min);
        minutesSpinner = new JSpinner(minutesModel);
        minutesSpinner.setEditor(new JSpinner.NumberEditor(minutesSpinner, "#"));
        minutesSpinner.addChangeListener(this);
        minutesSpinner.setMaximumSize(SMALL);

        final Box challengeOpt = new Box(BoxLayout.LINE_AXIS);
        challengeOpt.setAlignmentX(Box.LEFT_ALIGNMENT);
        challengeOpt.add(Box.createHorizontalStrut(30));
        challengeOpt.add(Box.createHorizontalGlue());
        challengeOpt.add(minutesSpinner);
        challengeOpt.add(challengeLabel);

        final Box styleBox = new Box(BoxLayout.PAGE_AXIS);
        styleBox.setAlignmentX(Box.LEFT_ALIGNMENT);
        styleBox.add(styleButtons[0]);
        styleBox.add(debugOpt);
        styleBox.add(styleButtons[1]);
        styleBox.add(styleButtons[2]);
        styleBox.add(styleButtons[3]);
        styleBox.add(challengeOpt);
        final Border styleBorder = BorderFactory.createTitledBorder(
                BorderFactory.createEtchedBorder(EtchedBorder.LOWERED),
                "Which style of play would you prefer?" );
        styleBox.setBorder(styleBorder);

        // JList for rules

        rulesList = new JList();
        rulesList.setAlignmentX(JList.LEFT_ALIGNMENT);
        rulesList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
        rulesList.setLayoutOrientation(JList.VERTICAL);
        final Border rulesBorder = BorderFactory.createLoweredBevelBorder();
        rulesList.setBorder(rulesBorder);

        // combine into a single Box

        final Box mainBox = new Box(BoxLayout.PAGE_AXIS);
        mainBox.add(styleBox);
        mainBox.add(Box.createVerticalStrut(10));
        mainBox.add(rulesList);

        add(mainBox);
    }

    // methods, sorted by name

    /**
     * Process an event indicating that a radio button changed.
     * @param event
     */
    @Override
    public void actionPerformed(java.awt.event.ActionEvent event) {
        assert event != null;

       final String command = event.getActionCommand();
        try {
            final GameStyle style = GameStyle.valueOf(command);
            gameOpt.setStyle(style);
        } catch (IllegalArgumentException exception) {
        }

        updateView();
    }

    private int getButtonIndex(GameStyle style)
            throws InvalidStyleException
    {
        switch (style) {
            case DEBUG:
                return 0;
            case PRACTICE:
                return 1;
            case FRIENDLY:
                return 2;
            case CHALLENGE:
                return 3;
            default:
                throw new InvalidStyleException();
        }
    }

    private String getButtonText(GameStyle style) {
        switch (style) {
            case DEBUG:
                return "Debug - allows peeking, undo, and hints";
            case PRACTICE:
                return "Practice - allows undo and hints, but no peeking";
            case FRIENDLY:
                return "Friendly - hints, but no undo or time limit";
            case CHALLENGE:
                return "Challenge - no undo or hints, time limited to";
            default:
                throw new AssertionError(style);
        }
    }

    private int getListIndex(Rules rules) {
        switch (rules) {
            case STANDARD:
                return 0;
            case CUSTOM:
                return 1;
            case REPLAY:
                return 2;
            default:
                throw new AssertionError(rules);
        }
    }

    private String[] getRulesData(boolean canReplay) {
        final int listCount = canReplay ? 3 : 2;

        final String[] result = new String[listCount];
        for (Rules rules : Rules.values()) {
            final int iList = getListIndex(rules);
            if (iList < listCount) {
                result[iList] = getRulesText(rules);
            }
        }

        return result;
    }

    private String getRulesText(Rules rules) {
        assert rules != null;

        switch (rules) {
            case CUSTOM:
                return "Custom Rules - you decide what the rules will be";
            case REPLAY:
                return "Replay - re-use the rules from the previous game";
            case STANDARD:
                return "Standard Rules (108 square tiles on an endless grid, 2 hands of 6)";
            default:
                throw new AssertionError(rules);
        }
    }

    @Override
    public String getTitle() {
        return "Create New Game";
    }

    /**
     * Process an event indicating that the "randomize" checkbox changed.
     * @param event
     */
    @Override
    public void itemStateChanged(java.awt.event.ItemEvent event) {
        final boolean randomize = randomizeBox.isSelected();
        gameOpt.setRandomize(randomize);
        updateView();
    }

    @Override
    public void nextAction() {
        updateModel();
        if (gameOpt.getRules().isCustom()) {
            showCard(ParmBox2.class.getName(), gameOpt, handOpts);
        } else {
            if (gameOpt.getRules().isStandard()) {
                gameOpt.standardize();
            }
            showCard(HandBox.class.getName(), gameOpt, handOpts);
        }
    }

    @Override
    public void setModels(Object[] models) {
        assert models != null;
        assert models.length == 2 : models.length;
        assert models[0] != null;
        assert models[0] instanceof GameOpt;
        assert models[1] != null;
        assert models[1] instanceof HandOpt[];

        gameOpt = (GameOpt)models[0];
        gameOpt.validate();
        handOpts = (HandOpt[])models[1];

        final boolean canReplay = gameOpt.getRules().isReplay();
        rulesList.setListData(getRulesData(canReplay));

        updateView();
    }

    /**
     * Process an event indicating that a spinner changed.
     * @param event
     */
    @Override
    public void stateChanged(javax.swing.event.ChangeEvent event) {
        updateModel();
    }

    private void updateModel() {
        // read list selection

        final int iRule = rulesList.getSelectedIndex();
        for (Rules rule : Rules.values()) {
           if (getListIndex(rule) == iRule) {
               gameOpt.setRules(rule);
               break;
           }
        }

        // read spinner values

        final Number minutes = (Number)minutesModel.getValue();
        gameOpt.setMinutesPerHand(minutes.intValue());

        final Number seed = (Number)seedModel.getValue();
        gameOpt.setSeed(seed.longValue());

        gameOpt.validate();
    }

    private void updateView() {
        // the wizard's back/next buttons
        wizard.getBackButton().setVisible(false); // can't go back
        wizard.getNextButton().setText("Next \u21d2");
        wizard.getNextButton().setEnabled(true);

        // enable/disable controls
        final GameStyle style = gameOpt.getStyle();
        debugLabel.setEnabled(style.isDebug());
        randomizeBox.setEnabled(style.isDebug());
        seedSpinner.setEnabled(style.isDebug() && !gameOpt.isRandomized());
        challengeLabel.setEnabled(style.isChallenge());
        minutesSpinner.setEnabled(style.isChallenge());

        // select/deselect radio buttons
        try {
            final int iButton = getButtonIndex(style);
            styleButtons[iButton].setSelected(true);
        } catch (InvalidStyleException exception) {
            throw new AssertionError();
        }

        // select/deselect checkbox
        randomizeBox.setSelected(gameOpt.isRandomized());

        // set spinner values
        minutesSpinner.setValue(gameOpt.getMinutesPerHand());
        seedSpinner.setValue(gameOpt.getSeed());

        // set list selection
        final int iList = getListIndex(gameOpt.getRules());
        rulesList.setSelectedIndex(iList);
    }
}
