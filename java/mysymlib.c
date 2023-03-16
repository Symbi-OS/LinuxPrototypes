#include <jni.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>

JNIEXPORT jlong JNICALL Java_FooClass_checkElevate(JNIEnv *env, jobject obj) {
    jlong result;
    // This is the check elevate call
    return syscall(448, 1<<1);
}

JNIEXPORT jlong JNICALL Java_FooClass_elevate(JNIEnv *env, jobject obj) {
    jlong result;
    // This is the check elevate call
    return syscall(448, 1 | 1<<2);
}

JNIEXPORT jlong JNICALL Java_FooClass_lower(JNIEnv *env, jobject obj) {
    jlong result;
    // This is the check elevate call
    return syscall(448, 0);
}