#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef unsigned short u16;
typedef unsigned int u32;

struct idt_bits {
	u16		ist	: 3,
			zero	: 5,
			type	: 5,
			dpl	: 2,
			p	: 1;
} __attribute__((packed));

struct gate_struct {
	u16		offset_low;
	u16		segment;
	struct idt_bits	bits;
	u16		offset_middle;
	u32		offset_high;
	u32		reserved;
} __attribute__((packed));

union idt_desc{

  struct {
    uint64_t lo;
    uint64_t hi;
  } raw;

  struct {
    // Low 2 bytes
    uint16_t  lo_addr;

    // segment selector
    uint16_t  seg_sel;

    // IS
    // TODO: Break type and 0 up into 4 and 1 bits each.
    // https://wiki.osdev.org/Interrupt_Descriptor_Table#Gate_Descriptor_2
    unsigned ist : 3, zero0 : 5, type : 5, dpl : 2, p : 1;
    // Zero
    /* uint16_t zero: 8; */
    /* uint16_t type: 4; */
    /* uint16_t zero2: 1; */
    /* uint16_t dpl: 2; */
    /* uint16_t p:1; */

    uint16_t  mid_addr;
    uint32_t hi_addr;

    uint32_t res;
  } fields;
}__attribute__((packed));

static_assert(sizeof(union idt_desc) == sizeof(struct gate_struct), "Size of idt_desc is not correct");
static_assert(sizeof(union idt_desc) == 16, "Size of idt_desc is not correct");
static_assert(sizeof(struct gate_struct) == 16, "Size of idt_desc is not correct");

int main(int argc, char *argv[])
{
    struct gate_struct gate = {0};
    union idt_desc idt = {0};

    gate.bits.ist = 7;    
    idt.fields.ist = 7;

    // Compare the two structs to make sure they are the same.
    int ret = memcmp(&gate, &idt, sizeof(struct gate_struct));
    if (ret != 0) {
        printf("Structs are not the same!\n");
        return -1;
    } else {
        printf("Structs are the same!\n");
    }

    gate.offset_middle = 0x1234;
    idt.fields.mid_addr = 0x1234;

    ret = memcmp(&gate, &idt, sizeof(struct gate_struct));
    if (ret != 0) {
        printf("Structs are not the same!\n");
        return -1;
    } else {
        printf("Structs are the same!\n");
    }

    return 0;
}