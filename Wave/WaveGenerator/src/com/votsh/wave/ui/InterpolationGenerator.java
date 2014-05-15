/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.ui;

import com.votsh.wave.entries.LedColor;
import com.votsh.wave.entries.PixelsTimeline;
import com.votsh.wave.entries.Wave;
import com.votsh.wave.util.FileManager;
import com.votsh.wave.util.Pair;
import org.json.JSONArray;

/**
 *
 * @author Luis
 */
public class InterpolationGenerator {

    /**
     * General configuration
     */
    public final static int size = 16;
    public final static int framerate = 24;

    private final long initialMillisec;
    private int currentFrame;
    private final PixelsTimeline[] rgbValues;
    private final JSONArray interpolationTable;
    
    public Wave wave;

    public InterpolationGenerator(long millisec) throws Exception {
        interpolationTable = new JSONArray(FileManager.readString("InterpolationTable.json"));
        initialMillisec = millisec;
        rgbValues = new PixelsTimeline[size];
        initTimeColorPairs = new Pair[size];
        nextTimeColorsPairs = new Pair[size];
        index = new int[size];
    }

    //Data
    final private Pair<Integer, LedColor>[] initTimeColorPairs;
    final private Pair<Integer, LedColor>[] nextTimeColorsPairs;

    //Meta data where I am
    final private int[] index;

    /**
     * Setup the wave system
     *
     * @param theWave
     */
    public void setup(Wave theWave) {
        wave = theWave;
        for (int pixelIndex = 0; pixelIndex < size; pixelIndex++) {
            rgbValues[pixelIndex] = wave.timelines[pixelIndex / 4][pixelIndex % 4];
            index[pixelIndex] = 1;
            initTimeColorPairs[pixelIndex] = rgbValues[pixelIndex].timeline.get(0);
            nextTimeColorsPairs[pixelIndex] = rgbValues[pixelIndex].timeline.get(1);
        }
    }

    /**
     * Interpolation algorithm
     *
     * @return
     */
    public LedColor[] generateCurrentColors() throws Exception {
        LedColor[] newColors = new LedColor[size];
        currentFrame = getCurrentMillisec();
        for (int pixelIndex = 0; pixelIndex < size; pixelIndex++) {
            increaseIndex(pixelIndex);
            newColors[pixelIndex] = findInterpolation(pixelIndex);
        }

        return newColors;
    }

    private void increaseIndex(int pixelIndex) {
        Pair<Integer, LedColor> nextColor = nextTimeColorsPairs[pixelIndex];
        if (nextColor.first < currentFrame && index[pixelIndex]<
                rgbValues[pixelIndex].timeline.size()-1) {
            initTimeColorPairs[pixelIndex] = nextTimeColorsPairs[pixelIndex];
            index[pixelIndex] = index[pixelIndex]+1;
            nextTimeColorsPairs[pixelIndex]
                    = rgbValues[pixelIndex].timeline.get(index[pixelIndex]);
        }
    }

    private LedColor findInterpolation(int pixelIndex) throws Exception {
        Pair<Integer, LedColor> initTimeColorPair = initTimeColorPairs[pixelIndex];
        Pair<Integer, LedColor> nextTimeColorPair = nextTimeColorsPairs[pixelIndex];
        int initFrame = initTimeColorPair.first;
        int nextFrame = nextTimeColorPair.first;
        //Following nomenclature
        LedColor I0 = initTimeColorPair.second;
        LedColor I3 = nextTimeColorPair.second;

        //Following documentation nomenclature
        if (nextFrame == initFrame ){
           return nextTimeColorPair.second;
        }
        int bt = ((currentFrame - initFrame) * 10000) / (nextFrame - initFrame);
        if (bt > 10000) bt = 10000;
        
        LedColor newColor = new LedColor();
        newColor.r = findInterpolation(bt, I0.r, I3.r);
        newColor.g = findInterpolation(bt, I0.g, I3.g);
        newColor.b = findInterpolation(bt, I0.b, I3.b);
        
        return newColor;
    }

    private double findInterpolation(int bt, double I0, double I3) throws Exception {
        JSONArray BI0t_BI3t = interpolationTable.getJSONArray(bt);
        int BI0t = BI0t_BI3t.getInt(0);
        int BI3t = BI0t_BI3t.getInt(1);
        double BI = (I0 * BI0t + I3 * BI3t)/10000;
        if (BI > 1){
            int i = 34;
        }
        return BI;
    }

    private int getCurrentMillisec() {
        return (int)(System.currentTimeMillis()-initialMillisec);
    }


}
