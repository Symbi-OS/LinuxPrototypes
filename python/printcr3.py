import ctypes
symlib = ctypes.cdll.LoadLibrary('../../Symlib/dynam_build/libSym.so')

print("Hello World")

my_module = ctypes.cdll.LoadLibrary('./my_shared_object.so')
# rax = my_module.get42()

# DO_ELEVATED = False
DO_ELEVATED = True

if DO_ELEVATED:
    symlib.sym_elevate()

rax = my_module.getcr3()

if DO_ELEVATED:
    symlib.sym_lower()

print("RAX = 0x{:x}".format(rax))
