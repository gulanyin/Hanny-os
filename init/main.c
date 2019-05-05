#include "os/print_h.h"

struct mm {
    unsigned int add_low;
    unsigned int add_hig;
    unsigned int len_low;
    unsigned int len_hig;
    unsigned int type;

};

void main(){

    unsigned int mem_number;
    int i;

    struct mm *p_mm;

    print_char('M');
    print_str("main function\n");
    print_int_oct(100);
    print_char('\n');

    mem_number = *((int*)(0x85000));

    p_mm = (struct mm *)(0x85010);

    print_int_oct(       mem_number     );
    print_char('\n');

    for (i=0; i<mem_number; i++){
        print_str("addr ");
        print_int_oct(       (p_mm + i)->add_low     );
        print_str("   len ");
        print_int_oct(       (p_mm + i)->len_hig     );
        print_int_oct(       (p_mm + i)->len_low     );
        print_str("   type ");
        print_int_oct(       (p_mm + i)->type     );

        print_char('\n');
    }

}
