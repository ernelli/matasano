default: all

all: tools.o test1 test2 test3 test4 test5 test6 test7 test7a test8 \
     test9 test10 test11 test12 test13 test14 test15 test16 \
     test17 test18 test19 test20 test21 test22 test23 test24 test24b \
     test25 test26 test27 test28 test29


challenge1: base64.o rate.o test1 test2 test3 test4 test5 test6 test7 test8
	@echo "// ------------------------------------------------------------\nrunning test1"
	@echo -n "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d" | ./test1
	@echo "// ------------------------------------------------------------\nrunning test2"
	@echo "1c0111001f010100061a024b53535009181c\n686974207468652062756c6c277320657965" | ./test2
	@echo "// ------------------------------------------------------------\nrunning test3"
	@echo "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736" | ./test3
	@echo "// ------------------------------------------------------------\nrunning test4"
	@cat gistfile1.txt | ./test4
	@echo "// ------------------------------------------------------------\nrunning test5"
	@cat data5.txt | ./test5
	@echo "// ------------------------------------------------------------\nrunning test6"
	@cat gistfile2.txt | ./test6
	@echo "// ------------------------------------------------------------\nrunning test7"
	@cat gistfile3.txt | ./test7
	@echo "// ------------------------------------------------------------\nrunning test8"
	@cat gistfile4.txt | ./test8

challenge2: base64.o rate.o test9 test10 test11 test12 test13 test14 test15 test16
	@echo "// ------------------------------------------------------------\nrunning test9"
	@echo -n "YELLOW SUBMARINE" | ./test9
	@echo "// ------------------------------------------------------------\nrunning test10"
	@cat gistfile5.txt | ./test10
	@echo "// ------------------------------------------------------------\nrunning test11"
	@./test11
	@echo "// ------------------------------------------------------------\nrunning test12"
	@./test12
	@echo "// ------------------------------------------------------------\nrunning test13"
	@./test13
	@echo "// ------------------------------------------------------------\nrunning test14"
	@./test14
	@echo "// ------------------------------------------------------------\nrunning test15"
	@./test15
	@echo "// ------------------------------------------------------------\nrunning test16"
	@./test16

challenge3: tools.o test17 test18 test19 test20 test21 test22 test23 test24 test24b
	@echo "// ------------------------------------------------------------\nrunning test17"
	@./test17
	@echo "// ------------------------------------------------------------\nrunning test18"
	@./test18
	@echo "// ------------------------------------------------------------\nrunning test19"
	@./test19
	@echo "// ------------------------------------------------------------\nrunning test20"
	@cat ./gistfile6.txt | ./test20
	@echo "// ------------------------------------------------------------\nrunning test21"
	@./test21
	@echo "// ------------------------------------------------------------\nrunning test22 using a pregenerated random number, and start time"
	@./test22 2418051504 1374175866
	@echo "// ------------------------------------------------------------\nrunning test23"
	@./test23
	@echo "// ------------------------------------------------------------\nrunning test24a"
	@./test24
	@echo "// ------------------------------------------------------------\nrunning test24a"
	@./test24b

challenge4: tools.o test25.o
	cat gistfile3.txt | ./test25

clean:
	rm *.o

tools.o : tools.c tools.h
	gcc -c -Wall tools.c

test1: test1.o tools.o
	gcc -o test1 test1.o tools.o

test2: test2.o tools.o
	gcc -o test2 test2.o tools.o

test3: test3.o tools.o 
	gcc -o test3 test3.o tools.o 

test4: test4.o tools.o 
	gcc -o test4 test4.o tools.o 

test5: test5.o tools.o 
	gcc -o test5 test5.o tools.o 

test6: test6.o tools.o 
	gcc -o test6 test6.o tools.o 

test7: test7.o tools.o
	gcc -o test7 test7.o tools.o -l crypto

test7a: test7a.o tools.o
	gcc -o test7a test7a.o tools.o


test8: test8.o tools.o
	gcc -o test8 test8.o tools.o

#####

test9: test9.o tools.o
	gcc -o test9 test9.o tools.o

test10: test10.o tools.o
	gcc -o test10 test10.o tools.o

test11: test11.o tools.o oracle.o
	gcc -o test11 test11.o tools.o oracle.o

test12: test12.o tools.o oracle.o
	gcc -o test12 test12.o tools.o oracle.o

test13: test13.o tools.o
	gcc -o test13 test13.o tools.o

test14: test14.o tools.o oracle.o
	gcc -o test14 test14.o tools.o oracle.o

test15: test15.o tools.o
	gcc -o test15 test15.o tools.o 

test16: test16.o tools.o
	gcc -o test16 test16.o tools.o 

test17: test17.o tools.o oracle.o
	gcc -o test17 test17.o tools.o oracle.o

test18: test18.o tools.o
	gcc -o test18 test18.o tools.o

test19: test19.o tools.o
	gcc -o test19 test19.o tools.o

test20: test20.o tools.o
	gcc -o test20 test20.o tools.o

test21: test21.o tools.o
	gcc -o test21 test21.o tools.o

test22: test22.o tools.o
	gcc -o test22 test22.o tools.o

test23: test23.o tools.o
	gcc -o test23 test23.o tools.o

test24: test24.o tools.o
	gcc -o test24 test24.o tools.o

test24b: test24b.o tools.o
	gcc -o test24b test24b.o tools.o

test25: test25.o tools.o
	gcc -o test25 test25.o tools.o

test26: test26.o tools.o
	gcc -o test26 test26.o tools.o

test27: test27.o tools.o
	gcc -o test27 test27.o tools.o

test28: test28.o tools.o
	gcc -o test28 test28.o tools.o

test29: test29.o tools.o
	gcc -o test29 test29.o tools.o
