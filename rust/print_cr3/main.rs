use std::arch::asm;
// function foo that returns a u64
fn bar() -> u64 {
    let mut x: u64;
    unsafe {
        asm!("mov {}, cr3", out(reg) x);
    }
    return x;
}

fn main() {
    let cr3 = bar();
    println!("CR3: {:x}", cr3);
}