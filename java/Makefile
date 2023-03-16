JAVA_HOME=/usr/lib/jvm/java-17-openjdk-17.0.6.0.10-1.fc36.x86_64/

all: Main.class libmylibrary.so

Main.class: Main.java MyClass.class FooClass.class
	javac Main.java

MyClass.class: MyClass.java
	javac MyClass.java

FooClass.class: FooClass.java libFoo.so
	javac $<

INCLUDES=-I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux
SOFLAGS=-shared -fPIC
libmylibrary.so: mylibrary.c
	gcc $(SOFLAGS) $(INCLUDES) $< -o $@

libFoo.so: mysymlib.c
	gcc $(SOFLAGS) $(INCLUDES) $< -o $@

run: Main.class
	java Main

clean:
	rm -f MyClass.class libmylibrary.so *err* *.class libFoo.so

# prefix_syms: libSym_mod.so
# 	objcopy --prefix-symbols=Java_MyClass_ libSym_mod.so


# all:
# 	javac MyClass.java
# # javac -h -jni MyClass

# lib:
# 	gcc -shared -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux mylibrary.c -o libmylibrary.so

# run:
# 	java Main
# run2:
# 	java Main MyClass

# be:
# 	shortcut.sh -be  --- java Main
# ###