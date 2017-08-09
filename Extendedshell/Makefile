shell: main.o echo.o exec_command.o redirection.o piping.o prompt.o
	gcc $^ -o $@	
main.o : main.c
	gcc -c main.c
echo.o:  echo.c echo.h
	gcc -c echo.c
exec_command.o:exec_command.c exec_command.h
	gcc -c exec_command.c
redirection.o:redirection.c redirection.h
	gcc -c redirection.c
piping.o:piping.c piping.h
	gcc -c piping.c
prompt.o:prompt.c prompt.h
	gcc -c prompt.c
clean:
	rm -f *.o shell
