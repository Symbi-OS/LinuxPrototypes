run: my_shared_object.so
	python3 printcr3.py

# Link the object file into a shared object using the GNU Compiler Collection (GCC)
my_shared_object.so: my_assembly_function.o
	gcc -shared -o my_shared_object.so my_assembly_function.o

# Assemble the code using the GNU Assembler (GAS) into an object file
my_assembly_function.o: my_assembly_function.S
	as -o my_assembly_function.o my_assembly_function.S

clean:
	rm -f *.o *.so