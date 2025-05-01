# main: main.obj HLASKY.obj
# gcc -std=c99 -o main main.obj HLASKY.H
# main.obj: main.c
# gcc -std=c99 -c -o main.obj main.c
# HLASKY.obj: HLASKY.H
# 	gcc -std=c99 -c -o HLASKY.obj -x c-header HLASKY.H

main: main.c HLASKY.H
	gcc -std=c99 -w -g -ggdb -o main main.c -x c-header HLASKY.H
