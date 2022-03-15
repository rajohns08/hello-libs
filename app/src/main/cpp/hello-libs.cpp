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
#include <openssl/evp.h>
#include <openssl/digest.h>
#include <string>
#include <openssl/aes.h>

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
    auto *buffer = new uint8_t[num_bytes];
    RAND_bytes(buffer, num_bytes);
    jbyteArray ret = env->NewByteArray(num_bytes);
    env->SetByteArrayRegion(ret, 0, num_bytes, (jbyte*)buffer);
    return ret;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_example_hellolibs_MainActivity_pbkdf2(JNIEnv *env, jobject thiz, jstring password, jbyteArray salt, jint iterations) {
    const char *nativePassword = env->GetStringUTFChars(password, nullptr);
    size_t password_len = strlen(nativePassword);

    jbyte *nativeSalt = env->GetByteArrayElements(salt, nullptr);
    size_t salt_len = env->GetArrayLength(salt);

    size_t key_len = 32;
    auto *out_key = new uint8_t[key_len];

    PKCS5_PBKDF2_HMAC(nativePassword, password_len,
                      (uint8_t*)nativeSalt, salt_len,
                      (unsigned)iterations, EVP_sha256(),
                      key_len, out_key);

    jbyteArray ret = env->NewByteArray(key_len);
    env->SetByteArrayRegion(ret, 0, key_len, (jbyte*)out_key);

    env->ReleaseByteArrayElements(salt, nativeSalt, 0);

    return ret;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_example_hellolibs_MainActivity_crypt(JNIEnv *env, jobject thiz, jbyteArray data, jbyteArray key, jint operation) {
    jbyte *nativeData = env->GetByteArrayElements(data, nullptr);
    jbyte *nativeKey = env->GetByteArrayElements(key, nullptr);

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();

    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), nullptr, (uint8_t*)nativeKey, nullptr, (int)operation);

    size_t data_len = env->GetArrayLength(data);
    size_t buf_size = data_len + AES_BLOCK_SIZE;
    auto *output = new uint8_t[buf_size];
    int output_len;
    int len;

    EVP_CipherUpdate(ctx, output, &len, (uint8_t*)nativeData, data_len);
    output_len = len;

    EVP_CipherFinal_ex(ctx, output + len, &len);
    output_len += len;

    EVP_CIPHER_CTX_free(ctx);

    jbyteArray ret = env->NewByteArray(output_len);
    env->SetByteArrayRegion(ret, 0, output_len, (jbyte*)output);

    env->ReleaseByteArrayElements(data, nativeData, 0);
    env->ReleaseByteArrayElements(key, nativeKey, 0);

    return ret;
}
