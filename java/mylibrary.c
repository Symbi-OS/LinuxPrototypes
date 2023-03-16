#include <jni.h>
JNIEXPORT jlong JNICALL Java_MyClass_readRax(JNIEnv *env, jobject obj) {
    jlong result;
    __asm__("movq %%rax, %0;" : "=r" (result));
    result = 42;
    return result;
}

extern long sym_check_elevate();
