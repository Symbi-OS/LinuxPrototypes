public class MyClass {
    public native long readRax();
    public native long sym_check_elevate();
    static {
        // print loading lib
        System.out.println("Loading mylibrary");
        System.loadLibrary("mylibrary");

        System.out.println("Loading symlib");
        System.loadLibrary("Sym");
    }
}