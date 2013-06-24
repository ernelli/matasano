default: challenge2

all: base64.o rate.o tools.o test1 test2 test3 test4 test5 test6 test7 test7a test8 \
	test9 test10 test11 test12 test13 test14 test15 test16

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

clean:
	rm *.o

test1: test1.o base64.o
	gcc -o test1 test1.o base64.o

test2: test2.o base64.o
	gcc -o test2 test2.o base64.o

test3: test3.o base64.o rate.o
	gcc -o test3 test3.o base64.o rate.o

test4: test4.o base64.o rate.o
	gcc -o test4 test4.o base64.o rate.o

test5: test5.o base64.o rate.o
	gcc -o test5 test5.o base64.o rate.o

test6: test6.o base64.o rate.o
	gcc -o test6 test6.o base64.o rate.o

test7: test7.o tools.o
	gcc -o test7 test7.o tools.o -l crypto

test7a: test7a.o tools.o
	gcc -o test7a test7a.o tools.o


test8: test8.o base64.o
	gcc -o test8 test8.o base64.o

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