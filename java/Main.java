
public class Main {
    public static void main(String[] args) {
        MyClass myClass = new MyClass();

        long raxValue = myClass.readRax();
        System.out.println("The value of rax is: " + Long.toHexString(raxValue));
        myClass.sym_check_elevate();
        // myClass.sym_lower();
    }
}
