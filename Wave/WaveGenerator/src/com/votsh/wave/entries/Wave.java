/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.entries;

import org.json.JSONArray;
import org.json.JSONObject;

/**
 *
 * @author Luis
 */
public class Wave {

    public final static int size = 4;
    public final static int inchSize = 10;
    

    public final static double inchLedSeparation = ((double) inchSize) / ((double) size);

    public String name;
    //
    // * 30 31 32 33
    // * 20 21 22 23
    // * 10 11 12 13
    // * 00 01 02 03
    // 
    public PixelsTimeline[][] timelines;

    /**
     * X and Y of the front left corner in inches
     */
    public int X, Y;

    //Rotation clockwise on the front left corner on degrees
    public double rotation;

    public Wave() {
        timelines = new PixelsTimeline[size][size];
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                timelines[i][j]= new PixelsTimeline(); 
            }
        }
    }
    
    public void trimTimelines(int finalMillisecond){
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                timelines[i][j] .trimTimelines(finalMillisecond);
            }
        }
    }

    @Override
    public String toString() {
        return name + ": X=" + X + " Y=" + Y + " Rot=" + rotation;
    }

    /**
     * Adds to json timeline
     * @return
     * @throws Exception 
     */
    public JSONObject toJSON() throws Exception {
        JSONObject jsonWave = new JSONObject();
        jsonWave.put("name", name);
        JSONArray jsonTimeline = new JSONArray();
        jsonWave.put("timeline", jsonTimeline);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                PixelsTimeline pixelTimeline = timelines[i][j];
                jsonTimeline.put(pixelTimeline.toJSON());
            }
        }
        return jsonWave;
    }
}
