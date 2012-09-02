// File:     ParmBox3.java
// Location: Java/GoldTile/src/goldtile
// Purpose:  ParmBox3 class for the Gold Tile Game
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

public class ParmBox3
    extends WizardCard
    implements javax.swing.event.ChangeListener  // spinners
{
    // constants, sorted by type
    final private static Dimension SMALL = new Dimension(45, 20);
    final private static Fraction BONUS_FRACTION_MAX = new Fraction(0.25);
    final private static int CLONES_PER_COMBO_MAX = 15;
    final private static int HAND_SIZE_MAX = 20;
    final public static int HANDS_DEALT_MAX = 10;

    // embedded classes

    private static class AttrRow
        extends Box
    {
        final private JSpinner spinner;
        final private SpinnerNumberModel model;

        public AttrRow(ParmBox3 parent, int iAttr) {
            super(BoxLayout.LINE_AXIS);

            model = new SpinnerNumberModel(
                    Attr.COUNT_DEFAULT,
                    Attr.COUNT_MIN,
                    Attr.COUNT_MAX,
                    1);
            spinner = new JSpinner(model);
            spinner.setEditor(new JSpinner.NumberEditor(spinner, "#"));
            spinner.addChangeListener(parent);
            spinner.setMaximumSize(SMALL);

            final String text = String.format("The %s attribute takes ",
                    StringExt.ordinal(1 + iAttr));

            setAlignmentX(Box.LEFT_ALIGNMENT);
            add(Box.createHorizontalStrut(30));
            add(new JLabel(text));
            add(spinner);
            add(new JLabel(" possible values."));
        }

        public int getValue() {
            return model.getNumber().intValue();
        }

        public void setValue(int attrCount) {
            model.setValue(attrCount);
        }
    }

    // per-instance fields, sorted by type
    private GameOpt gameOpt = null;
    private HandOpt[] handOpts = null;
    final private AttrRow[] attrRows = new AttrRow[GameOpt.ATTR_COUNT_MAX];
    final private JLabel clonesPerComboLabel = new JLabel();
    final private JLabel handsLabel = new JLabel();
    final private JLabel tilesLabel = new JLabel();
    final private JLabel totalCombosLabel = new JLabel();
    final private JLabel totalTilesLabel = new JLabel();
    final private SpinnerNumberModel attrCountModel;
    final private SpinnerNumberModel clonesModel;
    final private SpinnerNumberModel handSizeModel;
    final private SpinnerNumberModel handsDealtModel;
    final private SpinnerNumberModel percentModel;

    // constructors

    @SuppressWarnings("LeakingThisInConstructor")
    public ParmBox3(Wizard wizard) {
        super(wizard, ParmBox3.class.getName());

        // number of hands

        handsDealtModel = new SpinnerNumberModel(
                GameOpt.HANDS_DEALT_DEFAULT,
                GameOpt.HANDS_DEALT_MIN,
                HANDS_DEALT_MAX,
                1);
        final JSpinner handsSpinner = new JSpinner(handsDealtModel);
        handsSpinner.setEditor(new JSpinner.NumberEditor(handsSpinner, "#"));
        handsSpinner.addChangeListener(this);
        handsSpinner.setMaximumSize(SMALL);

        // tiles per hand

        handSizeModel = new SpinnerNumberModel(
                GameOpt.HAND_SIZE_DEFAULT,
                GameOpt.HAND_SIZE_MIN,
                HAND_SIZE_MAX,
                1);
        final JSpinner tilesSpinner = new JSpinner(handSizeModel);
        tilesSpinner.setEditor(new JSpinner.NumberEditor(tilesSpinner, "#"));
        tilesSpinner.addChangeListener(this);
        tilesSpinner.setMaximumSize(SMALL);

        final Box handsRow = new Box(BoxLayout.LINE_AXIS);
        handsRow.add(new JLabel("Deal "));
        handsRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        handsRow.add(handsSpinner);
        handsRow.add(handsLabel);
        handsRow.add(tilesSpinner);
        handsRow.add(tilesLabel);

        // attributes per tile

        attrCountModel = new SpinnerNumberModel(
                GameOpt.ATTR_COUNT_DEFAULT,
                GameOpt.ATTR_COUNT_MIN,
                GameOpt.ATTR_COUNT_MAX,
                1);
        final JSpinner attrCountSpinner = new JSpinner(attrCountModel);
        attrCountSpinner.setEditor(new JSpinner.NumberEditor(
                attrCountSpinner, "#"));
        attrCountSpinner.addChangeListener(this);
        attrCountSpinner.setMaximumSize(SMALL);

        final Box attrCountRow = new Box(BoxLayout.LINE_AXIS);
        attrCountRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        attrCountRow.add(new JLabel("Tiles have "));
        attrCountRow.add(attrCountSpinner);
        attrCountRow.add(new JLabel(" attributes:"));

        // values per attribute

        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            attrRows[iAttr] = new AttrRow(this, iAttr);
        }

        final Box totalCombosRow = new Box(BoxLayout.LINE_AXIS);
        totalCombosRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        totalCombosRow.add(Box.createHorizontalStrut(60));
        totalCombosRow.add(totalCombosLabel);

        // clones per combo

        clonesModel = new SpinnerNumberModel(
                GameOpt.CLONES_PER_COMBO_DEFAULT,
                GameOpt.CLONES_PER_COMBO_MIN,
                CLONES_PER_COMBO_MAX,
                1);
        final JSpinner clonesSpinner = new JSpinner(clonesModel);
        clonesSpinner.setEditor(new JSpinner.NumberEditor(clonesSpinner, "#"));
        clonesSpinner.addChangeListener(this);
        clonesSpinner.setMaximumSize(SMALL);

        final Box clonesRow = new Box(BoxLayout.LINE_AXIS);
        clonesRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        clonesRow.add(new JLabel("Add "));
        clonesRow.add(clonesSpinner);
        clonesRow.add(clonesPerComboLabel);

        final Box totalTilesRow = new Box(BoxLayout.LINE_AXIS);
        totalTilesRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        totalTilesRow.add(Box.createHorizontalStrut(60));
        totalTilesRow.add(totalTilesLabel);

        // percent bonus tiles

        percentModel = new SpinnerNumberModel(
                GameOpt.BONUS_FRACTION_DEFAULT.toPercent(),
                GameOpt.BONUS_FRACTION_MIN.toPercent(),
                BONUS_FRACTION_MAX.toPercent(),
                1.0);
        final JSpinner percentSpinner = new JSpinner(percentModel);
        percentSpinner.setEditor(new JSpinner.NumberEditor(percentSpinner, "#"));
        percentSpinner.addChangeListener(this);
        percentSpinner.setMaximumSize(SMALL);

        final Box percentRow = new Box(BoxLayout.LINE_AXIS);
        percentRow.setAlignmentX(Box.LEFT_ALIGNMENT);
        percentRow.add(percentSpinner);
        percentRow.add(new JLabel("% of tiles have bonus value."));

        // combine into a single Box

        final Box mainBox = new Box(BoxLayout.PAGE_AXIS);
        mainBox.add(handsRow);
        mainBox.add(Box.createVerticalStrut(8));
        mainBox.add(attrCountRow);
        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            mainBox.add(attrRows[iAttr]);
        }
        mainBox.add(totalCombosRow);
        mainBox.add(Box.createVerticalStrut(8));
        mainBox.add(clonesRow);
        mainBox.add(totalTilesRow);
        mainBox.add(Box.createVerticalStrut(8));
        mainBox.add(percentRow);

        add(mainBox);
    }

    // methods, sorted by name

    @Override
    public void backAction() {
        updateModel();
        showCard(ParmBox2.class.getName(), gameOpt, handOpts);
    }


    @Override
    public String getTitle() {
        return "Hand and Tile Parameters";
    }

    @Override
    public void nextAction() {
        updateModel();
        showCard(HandBox.class.getName(), gameOpt, handOpts);
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

        updateView();
    }

    @Override
    public void stateChanged(javax.swing.event.ChangeEvent event) {
        // from a spinner
        updateModel();
    }

    private void updateModel() {
        // read spinner values

        final Number handsDealt = (Number)handsDealtModel.getValue();
        gameOpt.setHandsDealt(handsDealt.intValue());

        final Number handSize = (Number)handSizeModel.getValue();
        gameOpt.setHandSize(handSize.intValue());

        final Number attrCount = (Number)attrCountModel.getValue();
        gameOpt.setAttrCount(attrCount.intValue());

        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            final int valueCount = attrRows[iAttr].getValue();
            gameOpt.setAttrValueCount(iAttr, valueCount);
        }

        final Number clonesPerCombo = (Number)clonesModel.getValue();
        gameOpt.setClonesPerCombo(clonesPerCombo.intValue());

        final Number percent = (Number)percentModel.getValue();
        gameOpt.setBonusFraction(new Fraction(percent.doubleValue() / 100.0));

        gameOpt.validate();

        updateView();
    }

    private void updateView() {
        // the wizard's back/next buttons
        wizard.getBackButton().setText("\u21d0 Back");
        wizard.getBackButton().setEnabled(true);
        wizard.getBackButton().setVisible(true);
        wizard.getNextButton().setText("Next \u21d2");
        wizard.getNextButton().setEnabled(true);

        // set labels, spinner values, and visibility
        final int handsDealt = gameOpt.getHandsDealt();
        handsDealtModel.setValue(handsDealt);
        final String handsText = String.format(" hand%s of ",
                StringExt.plural(handsDealt));
        handsLabel.setText(handsText);

        final int handSize = gameOpt.getHandSize();
        handSizeModel.setValue(handSize);
        final String tilesText = String.format(" tile%s%s.",
                StringExt.plural(handSize),
                handsDealt > 1 ? " each" : "");
        tilesLabel.setText(tilesText);

        final int attrCount = gameOpt.getAttrCount();
        attrCountModel.setValue(attrCount);
        for (int iAttr = 0; iAttr < GameOpt.ATTR_COUNT_MAX; iAttr++) {
            final int valueCount = gameOpt.getAttrValueCount(iAttr);
            attrRows[iAttr].setValue(valueCount);
            attrRows[iAttr].setVisible(iAttr < attrCount);
        }

        final String totalCombosText = String.format(
                "\u21d2 %d attribute combinations",
                gameOpt.countCombos());
        totalCombosLabel.setText(totalCombosText);

        final int clonesPerCombo = gameOpt.getClonesPerCombo();
        clonesModel.setValue(clonesPerCombo);

        final String clonesText = String.format(" clone%s of each combination",
                StringExt.plural(clonesPerCombo));
        clonesPerComboLabel.setText(clonesText);

        final String totalTilesText = String.format("\u21d2 %d tiles in total",
                gameOpt.countTiles());
        totalTilesLabel.setText(totalTilesText);

        percentModel.setValue(gameOpt.getBonusFraction().toPercent());
    }
}
