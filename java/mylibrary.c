#include <jni.h>
JNIEXPORT jlong JNICALL Java_MyClass_readRax(JNIEnv *env, jobject obj) {
    jlong result;
    __asm__("movq %%cr3, %0;" : "=r" (result));
    return result;
}