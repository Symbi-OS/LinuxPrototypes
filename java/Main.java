
public class Main {
    public static void main(String[] args) {
        MyClass myClass = new MyClass();


        FooClass fooClass = new FooClass();

        long elevate = fooClass.checkElevate();
        System.out.println("The value of elevate is: " + Long.toHexString(elevate));

        fooClass.elevate();
        long raxValue = myClass.readRax();
        fooClass.lower();

        System.out.println("The value of rax is: " + Long.toHexString(raxValue));

    }
}
