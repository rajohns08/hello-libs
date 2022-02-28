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
 *
 */
#include <cstring>
#include <jni.h>
#include <cinttypes>
#include <android/log.h>
#include <openssl/base.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <string>

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   app/src/main/java/com/example/hellolibs/MainActivity.java
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_hellolibs_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz, jstring pac_) {
    const char *pac = env->GetStringUTFChars(pac_, nullptr);

    unsigned  char c[MD5_DIGEST_LENGTH];
    int i;
    char dest[32]={0};
    FILE *fp;
    MD5_CTX mdContext;
    size_t bytes;
    unsigned char data[1024];

    fp = fopen(pac, "rb");
    MD5_Init (&mdContext);
    while(( bytes = fread(data, 1, 1024, fp)) != 0 )
        MD5_Update(&mdContext, data, bytes );
    MD5_Final(c, &mdContext );
    fclose(fp);

    for( i = 0 ; i < MD5_DIGEST_LENGTH ; i++ )
        sprintf(dest+i*2,"%02X",c[i]);

    env->ReleaseStringUTFChars(pac_, pac);

    return env->NewStringUTF(dest);
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_example_hellolibs_MainActivity_randomBytes(JNIEnv *env, jobject thiz, jint num_bytes) {
    auto* aes_key = new uint8_t[num_bytes];
    RAND_bytes(aes_key, num_bytes);
    jbyteArray ret = env->NewByteArray(num_bytes);
    env->SetByteArrayRegion(ret, 0, num_bytes, (jbyte*)aes_key);
    return ret;
}
