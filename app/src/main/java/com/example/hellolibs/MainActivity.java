/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.example.hellolibs;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
/*
 * Simple Java UI to trigger jni function. It is exactly same as Java code
 * in hello-jni.
 */
public class MainActivity extends AppCompatActivity {
    
    enum Operation {
        DECRYPT,
        ENCRYPT
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        TextView tv = new TextView(this);
        tv.setText( stringFromJNI(getApplicationContext().getPackageCodePath()) );
        setContentView(tv);
        byte[] salt = {0x01};
        byte[] key2 = pbkdf2("testme", salt, 100000);
        byte[] key = randomBytes(256);
        
        String saltHex = "0102030405060708090A0B0C0D0E0F1D";
        byte[] saltBytes = hexStringToByteArray(saltHex);
        Log.d("tagzzz", "test");
        

        byte[] encrypted = crypt(salt, key2, Operation.ENCRYPT.ordinal());

        byte[] decrypted = crypt(encrypted, key2, Operation.DECRYPT.ordinal());
        Log.d("tagzzz", "test");
    }
    public native String stringFromJNI(String pac);
    public native byte[] randomBytes(int numBytes);
    public native byte[] pbkdf2(String password, byte[] salt, int iterations);
    public native byte[] crypt(byte[] data, byte[] key, int operation);
    static {
        System.loadLibrary("hello-libs");
    }





    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

}
