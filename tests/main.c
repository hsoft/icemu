#include <stdio.h>

void test_shiftregister_main();
void test_decoder_main();
void test_circuit_main();

int main()
{
    test_shiftregister_main();
    test_decoder_main();
    test_circuit_main();
    printf("All tests passed.\n");
}
