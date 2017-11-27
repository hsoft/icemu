#include <stdio.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_pin_init();
void test_shiftregister_init();
void test_decoder_init();
void test_counter_init();
void test_circuit_init();

int main()
{
    CU_initialize_registry();
    test_pin_init();
    test_shiftregister_init();
    test_decoder_init();
    test_counter_init();
    test_circuit_init();
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
