/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.votsh.wave.entries;

import java.util.ArrayList;

/**
 *
 * @author Luis
 */
public class Show {
    public int durationMilliseconds;
    
    public ArrayList<Wave> waves;
    
    /**
     * removes timelines before 0 and after final second
     * @param finalMillisecond 
     */
    public void trimTimelines(int finalMillisecond){
        durationMilliseconds = finalMillisecond;
        for (Wave wave :waves){
            wave.trimTimelines(finalMillisecond);
        }
    }
}
