/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.effects;

import com.votsh.wave.entries.LedColor;
import com.votsh.wave.entries.PixelsTimeline;
import com.votsh.wave.entries.Wave;
import com.votsh.wave.util.Pair;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author Luis
 */
public class WaveEffect implements EffectGenerator {

    public int centerX;
    public int centerY;
    public double speed;
    public int length;
    public int total;
    public double duration;

    /**
     *
     * @param configuration centerX-in centerY-in speed-in/sec length-in total
     */
    @Override
    public void setup(HashMap<String, Object> configuration) {
        centerX = (Integer) configuration.get("centerX");
        centerY = (Integer) configuration.get("centerY");
        speed = (Double) configuration.get("speed");
        length = (Integer) configuration.get("length");
        total = (Integer) configuration.get("total");
        duration = ((double) length) / speed;
    }

    /**
     * Apply wave effect
     *
     * @param waves
     */
    @Override
    public void applyEffect(ArrayList<Wave> waves) {

        for (int i = 0; i < total; i++) {
            LedColor rgb = new LedColor(Math.random(), Math.random(), Math.random());
            double time = duration * ((double) i);
            for (Wave wave : waves) {
                applyEffect(wave, rgb, time);
            }
        }
    }

    //         /|
    //        / |
    //    z  /  |          
    //      /   | Y = j
    //     / o = angle
    //    /_____|
    //      X = i  
    private void applyEffect(Wave wave, LedColor rgb, double initTime) {
        for (int i = 0; i < Wave.size; i++) {
            for (int j = 0; j < Wave.size; j++) {
                double x = (i * Wave.inchLedSeparation) + Wave.inchLedSeparation / 2.0;
                double y = (j * Wave.inchLedSeparation) + Wave.inchLedSeparation / 2.0;
                //double z = Math.sqrt(x * x + y * y);
                /*double o = Math.atan(y / x);
                double newO = o + (wave.rotation * Math.PI / 180.0);
                double newX = Math.cos(newO) * z;
                double newY = Math.sin(newO) * y;*/
                Pair<Double, Double> xy = new Pair<>(
                        (double) wave.X + x, (double) wave.Y + y);
                PixelsTimeline ptl = wave.timelines[i][j];
                applyEffect(ptl, xy, rgb, initTime);
            }
        }
    }

    /**
     *
     * @param ptl
     * @param xy
     * @param rgb
     * @param initTime
     */
    private void applyEffect(PixelsTimeline ptl, Pair<Double, Double> xy,
            LedColor rgb, double initTime) {
        
        double distanceToPixel = Math.sqrt(
                Math.pow(centerX - xy.first, 2)
                + Math.pow(centerY - xy.second, 2));
        double reachPixelTime = initTime + (distanceToPixel / speed);
        int initFrame = (int) (reachPixelTime * 1000);
        if (ptl.timeline.isEmpty()) {
            ptl.timeline.add(new Pair<>(-11, new LedColor(0, 0, 0)));
        }
        LedColor preColor = ptl.timeline.get(ptl.timeline.size() - 1).second;
        ptl.timeline.add(new Pair<>(initFrame-10, preColor));

        ptl.timeline.add(new Pair<>(initFrame, rgb));
        int endTime = (int) ((reachPixelTime + duration) * 1000);
        LedColor finalColor = new LedColor(rgb.r*0.2, rgb.g*0.2, rgb.b*0.2);
        ptl.timeline.add(new Pair<>(endTime-10, finalColor));

    }

}
