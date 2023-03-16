public class FooClass {
    public native long checkElevate();
    public native long elevate();
    public native long lower();
    static {
        System.out.println("Loading symlib");
        System.loadLibrary("Foo");
    }
}