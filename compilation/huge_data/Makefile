all: huge_data small_data

huge_data: huge_data.c
	gcc -mcmodel=large -o huge_data huge_data.c

small_data: small_data.c
	gcc -mcmodel=large -o small_data small_data.c

clean:
	rm small_data huge_data
