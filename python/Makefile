run: getCr3.so
	python3 printcr3.py

# Link the object file into a shared object using the GNU Compiler Collection (GCC)
getCr3.so: getCr3.o
	gcc -shared $< -o $@

# Assemble the code using the GNU Assembler (GAS) into an object file
getCr3.o: getCr3.S
	as $< -o $@

clean:
	rm -f *.o *.so