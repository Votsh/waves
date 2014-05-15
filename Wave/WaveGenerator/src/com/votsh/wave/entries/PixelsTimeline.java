/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.votsh.wave.entries;

import com.votsh.wave.util.Pair;
import java.util.ArrayList;
import org.json.JSONArray;
import org.json.JSONObject;

/**
 *
 * @author Luis
 */
public class PixelsTimeline {
    public ArrayList<Pair<Integer,LedColor>> timeline;
    
    public PixelsTimeline(){
        timeline = new ArrayList<>();
    }
    
    public void trimTimelines(int finalMillisecond){
        timeline.get(0).first =0;
        timeline.get(timeline.size()-1).first = finalMillisecond;
    }
    
    /**
     * Transforms seconds into milliseconds and RGB float to int
     * @return
     * @throws Exception 
     */
    public JSONArray toJSON() throws Exception {
        JSONArray jsonTimeline = new JSONArray();
        for (Pair<Integer,LedColor>pixelEntry:timeline){
            //adding pixel entry
            JSONObject jsonPixelEntry = new JSONObject();
            jsonTimeline.put(jsonPixelEntry);
            
            //adding time in millisecond
            jsonPixelEntry.put("time",pixelEntry.first);
            
            //Adding rgb
            JSONArray jsonRGB = new JSONArray();
            jsonPixelEntry.put("rgb",jsonRGB);
            LedColor rgb = pixelEntry.second;
            jsonRGB.put(to255(rgb.r));
            jsonRGB.put(to255(rgb.g));
            jsonRGB.put(to255(rgb.b));
        }
        return jsonTimeline;
    }
    
    private int to255(double a){
        return (int)(Math.round(a*255));
    }
}
