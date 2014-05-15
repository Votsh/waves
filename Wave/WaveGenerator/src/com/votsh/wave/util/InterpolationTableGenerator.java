/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.util;

import org.json.JSONArray;

/**
 *
 * @author Luis
 */
public class InterpolationTableGenerator {
    
    int slides;
    double increment;

    /**
     *
     * @param theSlides
     */
    public void setup(int theSlides) {
        slides = theSlides;
        increment = 1.0 / (double) theSlides;
    }
    
    public JSONArray getInterpolationTable() throws Exception {
        JSONArray array = new JSONArray();
        for (double current = 0; current <= 1.0; current=current+increment) {
            JSONArray entry = new JSONArray();
            array.put(entry);
            double Bt = current;//following documentation nomenclarture
            Cubic F = new Cubic();//following documentation nomenclarture
            F.solve(0.4, -0.6, 1.2, -Bt);
            
            double t = F.x1;//following documentation nomenclarture
            //System.out.println(t);
            double B_I0_t = Math.pow(1 - t, 3) + 3 * t * Math.pow(1 - t, 2);
            double B_I3_t = 3 * t * t * (1 - t) + t * t * t;
            entry.put(fiveDigits(B_I0_t));
            entry.put(fiveDigits(B_I3_t));
        }
        return array;
    }
    
    public int fiveDigits(double entry){
        return (int)Math.round(entry*10000);
    }
    
    public static void main(String[] args)
            throws Exception {
        InterpolationTableGenerator itg = new InterpolationTableGenerator();
        itg.setup(10000);
        JSONArray array = itg.getInterpolationTable();
        FileManager f = new FileManager();
        f.saveToFile(array, "InterpolationTable.json");
    }
}
