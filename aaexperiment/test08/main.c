void print_char(unsigned char);
void print_str(unsigned char*);
void print_int_oct(unsigned int);
void idt_init();

void main(){
    print_char('a');
    print_char('b');
    print_char(' ');
    print_char('c');
    print_char('d');
    print_char('\n');
    print_char('e');
    print_char('f');
    print_char('g');
    print_char('k');
    print_char('M');
    print_char('\n');

    
    idt_init();

    print_str("\n\n\ndsfcksdfjdjfsdksfj;dsf\nckckkcend\n\n");

    print_int_oct(1);
    print_char('\n');
    print_int_oct(10);
    print_char('\n');

    print_int_oct(0xFFFFFFFF);
    print_char('\n');



    // for(i=0; i< 2000; i++){
    //     print_char('k');
    // }
}
