all: lab1

lab1: mysh.o parser.o interpreter.o
	@gcc mysh.o parser.o interpreter.o -o lab1

mysh.o: mysh.c
	@gcc -c mysh.c

parser.o: parser.c
	@gcc -c parser.c

interpreter.o: interpreter.c
	@gcc -c interpreter.c

run: all
	@./lab1

runTest: all
	@./lab1 tester.txt

clean:
	@echo "removed object and program files"
	@rm -f mysh.o parser.o interpreter.o lab1