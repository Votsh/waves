/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.util;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import javax.swing.text.DateFormatter;
import org.json.JSONArray;
import org.json.JSONException;

/**
 *
 * @author Luis
 */
public class Bezier {

    public static JSONArray getTranslationsList(int tmax) throws JSONException {
        JSONArray array = new JSONArray();
        double split = 1.0 / (double) tmax;
        double t = 0.0;
        for (int i = 0; i <= tmax; i++) {
            JSONArray tValues = new JSONArray();
            array.put(tValues);
            double OneMinusT = 1 - t;
            double OneMinusTSquare = OneMinusT * OneMinusT;
            double OneMinusTCubic = OneMinusTSquare * OneMinusT;
            double Tsquare = t * t;
            double Tcubic = Tsquare * t;
            tValues.put(fourDigist(OneMinusTCubic    +    3*OneMinusTSquare*t));
            tValues.put(fourDigist(3*OneMinusT*Tsquare    +    Tcubic));
            t += split;
        }
        return array;
    }

    public static double fourDigist(double a) {
        return (((double) Math.round(a * 10000)) / 10000.0);
    }

    public static double[] smoothColorList(double initialTone, double finalTone, int frames) {
        double[] tones = new double[frames + 1];
        double split = 1.0 / (double) frames;
        double t = 0.0;
        for (int i = 0; i <= frames; i++) {
            double OneMinusT = 1 - t;
            double OneMinusTSquare = OneMinusT * OneMinusT;
            double OneMinusTCubic = OneMinusTSquare * OneMinusT;
            double Tsquare = t * t;
            double Tcubic = Tsquare * t;

            double first = initialTone * OneMinusTCubic;
            double second = initialTone * 3 * OneMinusTSquare * t;
            double third = finalTone * 3 * OneMinusT * Tsquare;
            double fourth = finalTone * Tcubic;
            double bezier = first + second + third + fourth;
            t += split;
            tones[i] = bezier;
        }
        return tones;
    }
    /*
    public static Complex[] smoothColorList(double initialTone, double finalTone, int frames) {
        
    }*/

    public static void main(String[] args) throws Exception {
        SimpleDateFormat a = new SimpleDateFormat("yyyyMMddkkmmss");
        
        System.out.println(a.parse("20140507125032").getTime());
       /* for (double tone : Bezier.smoothColorList(0, 1, 100)) {
         System.out.println(tone);
         }*
        /*try (FileOutputStream fos = new FileOutputStream("bezier.dat")) {
            fos.write(Bezier.getTranslationsList(5000).toString().getBytes());
            fos.flush();
        }*/
    }
}
