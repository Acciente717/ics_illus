all: fp_add_single_single fp_add_single_double fp_sub_single_single fp_sub_single_double \
fp_mul_single_double fp_div_single_double fp_convert_single_double fp_convert_single_long_double \
fp_add_single_long_double fp_add_vector_double fp_convert_int_double

fp_add_single_single: fp_add_single_single.c
	gcc -S -Og -o fp_add_single_single.s fp_add_single_single.c

fp_add_single_double: fp_add_single_double.c
	gcc -S -Og -o fp_add_single_double.s fp_add_single_double.c

fp_sub_single_single: fp_sub_single_single.c
	gcc -S -Og -o fp_sub_single_single.s fp_sub_single_single.c

fp_sub_single_double: fp_sub_single_double.c
	gcc -S -Og -o fp_sub_single_double.s fp_sub_single_double.c

fp_mul_single_double: fp_mul_single_double.c
	gcc -S -Og -o fp_mul_single_double.s fp_mul_single_double.c

fp_div_single_double: fp_div_single_double.c
	gcc -S -Og -o fp_div_single_double.s fp_div_single_double.c

fp_convert_single_double: fp_convert_single_double.c
	gcc -S -Og -o fp_convert_single_double.s fp_convert_single_double.c

fp_convert_int_double: fp_convert_int_double.c
	gcc -S -Og -o fp_convert_int_double.s fp_convert_int_double.c

fp_convert_single_long_double: fp_convert_single_long_double.c
	gcc -S -Og -o fp_convert_single_long_double.s fp_convert_single_long_double.c

fp_add_single_long_double: fp_add_single_long_double.c
	gcc -S -Og -o fp_add_single_long_double.s fp_add_single_long_double.c

fp_add_vector_double: fp_add_vector_double.c
	gcc -S -Og -o fp_add_vector_double.s fp_add_vector_double.c -mavx

clean:
	rm fp_add_single_single.s fp_add_single_double.s fp_sub_single_single.s fp_sub_single_double.s
	rm fp_mul_single_double.s fp_div_single_double.s fp_convert_single_double.s fp_convert_single_long_double.s
	rm fp_add_single_long_double.s fp_add_vector_double.s fp_convert_int_double.s
