/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.generators;

import com.votsh.wave.entries.Wave;
import java.util.ArrayList;

/**
 *
 * @author Luis
 */
public class GridGenerator implements WaveGenerator {

    private int totalX;
    private int totalY;
    private int spacingX;
    private int spacingY;
    private double rotation;

    public void setup(int totalX, int totalY, int spacingX, int spacingY, double rotation) {
        this.totalX = totalX;
        this.totalY = totalY;
        this.spacingX = spacingX;
        this.spacingY = spacingY;
        this.rotation = rotation;
    }

    @Override
    public ArrayList<Wave> generateWaves() {
        ArrayList<Wave> waves = new ArrayList<>();
        for (int i = 0; i < totalX; i++) {
            for (int j = 0; j < totalY; j++) {
                Wave wave = new Wave();
                waves.add(wave);
                int waveId =  ((i * totalY )+ j);
                wave.X = i * (spacingX + Wave.inchSize);
                wave.Y = j * (spacingY + Wave.inchSize);
                wave.name = "Wave." + String.format("%03d",waveId);
                wave.rotation = rotation;
               
            }
        }
        return waves;
    }


}
