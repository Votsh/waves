/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.votsh.wave.effects;

import com.votsh.wave.entries.Wave;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author Luis
 */
public interface EffectGenerator {
    public void setup(HashMap<String,Object> configuration);
    public void applyEffect(ArrayList<Wave> waveGrid);
}
