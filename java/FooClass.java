public class FooClass {
    public native long foo();
    static {
        System.out.println("Loading symlib");
        System.loadLibrary("Sym");
    }
}