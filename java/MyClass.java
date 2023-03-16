public class MyClass {
    public native long readRax();
    static {
        // print loading lib
        System.out.println("Loading mylibrary");
        System.loadLibrary("mylibrary");

    }
}