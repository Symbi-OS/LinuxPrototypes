#include <jni.h>
JNIEXPORT jlong JNICALL Java_FooClass_foo(JNIEnv *env, jobject obj) {
    jlong result;
    __asm__("movq %%rax, %0;" : "=r" (result));
    result = 42;
    return result;
}