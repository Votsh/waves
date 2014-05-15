/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.votsh.wave.util;

import com.votsh.wave.entries.Wave;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.ArrayList;
import org.json.JSONArray;
import org.json.JSONObject;

/**
 *
 * @author Luis
 */
public class FileManager {

    public static void saveToFile(JSONArray array, String filename) throws Exception {
        try (FileOutputStream fos = new FileOutputStream(filename)) {
            fos.write(array.toString().getBytes());
            fos.flush();
        }
    }

    public static String readString(String filename) throws Exception {
        StringBuilder sb = new StringBuilder();
        byte[] data = new byte[1000];
        try (FileInputStream fos = new FileInputStream(filename)) {
            int s;
            while ((s = fos.read(data)) >= 0) {
                sb.append(new String(data, 0, s));
            }
        }
        return sb.toString();
    }

    public static void saveToFile(ArrayList<Wave> waves,String file) throws Exception {
        JSONObject jsonObj = new JSONObject();
        JSONArray jsonWaves = new JSONArray();
        jsonObj.put("entries", jsonWaves);
        for (Wave wave : waves) {
            jsonWaves.put(wave.toJSON());
        }

        try (FileOutputStream fos = new FileOutputStream(file)) {
            fos.write(jsonObj.toString(1).getBytes());
            fos.flush();
        }
    }
}
