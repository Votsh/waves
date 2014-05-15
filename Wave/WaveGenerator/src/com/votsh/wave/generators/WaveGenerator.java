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
public interface WaveGenerator {
    
    
    public ArrayList<Wave> generateWaves();
    
}
