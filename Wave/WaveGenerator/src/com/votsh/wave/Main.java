/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave;

import com.votsh.wave.effects.WaveEffect;
import com.votsh.wave.entries.Wave;
import com.votsh.wave.generators.GridGenerator;
import com.votsh.wave.ui.InterpolationGenerator;
import com.votsh.wave.util.FileManager;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author Luis
 */
public class Main {

    public static void main(String[] args) throws Exception {
        GridGenerator wg = new GridGenerator();
        wg.setup(10, 10, 2, 2, 0);
        ArrayList<Wave> waves = wg.generateWaves();

        HashMap<String, Object> config = new HashMap<>();

        /**
         * centerX = (Integer) configuration.get("centerX"); centerY = (Integer)
         * configuration.get("centerY"); speed = (Double)
         * configuration.get("speed"); length = (Integer)
         * configuration.get("length"); total = (Integer)
         * configuration.get("total");
         */
        config.put("centerX", 40);
        config.put("centerY", 40);
        config.put("speed", 20.0);
        config.put("length", 30);
        config.put("total", 10);

        WaveEffect we = new WaveEffect();
        we.setup(config);
        we.applyEffect(waves);
        FileManager.saveToFile(waves, "wave.dat");
        
        
    }
}
