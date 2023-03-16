
public class Main {
    public static void main(String[] args) {
        MyClass myClass = new MyClass();

        long raxValue = myClass.readRax();
        System.out.println("The value of rax is: " + Long.toHexString(raxValue));

        FooClass fooClass = new FooClass();

        System.out.println("About to call sym_check_elevate");
        long elevate = fooClass.foo();
        System.out.println("Done calling sym_check_elevate");

    }
}
