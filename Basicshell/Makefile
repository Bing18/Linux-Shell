shell: main.o echo.o exec_command.o exit_child.o
	gcc main.o echo.o exec_command.o exit_child.o -o shell	
main.o : main.c
	gcc -c main.c
echo.o:  echo.c header.h
	gcc -c echo.c
exec_command.o:exec_command.c header1.h
	gcc -c exec_command.c
exit_child.o:exit_child.c header2.h
	gcc -c exit_child.c

clean:
	rm -f *.o shell
