JAVA_HOME=/usr/lib/jvm/java-17-openjdk-17.0.6.0.10-1.fc36.x86_64/
# build: Main.java MyClass.java mylibrary.c
# 	# javac Main.java
# 	# javac MyClass.java

all: Main.class MyClass.class libmylibrary.so

Main.class: Main.java
	javac Main.java

MyClass.class: MyClass.java
	javac MyClass.java

libmylibrary.so: mylibrary.c
	gcc -shared -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux mylibrary.c -o libmylibrary.so

run:
	java Main

clean:
	rm -f MyClass.class libmylibrary.so *err* *.class


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