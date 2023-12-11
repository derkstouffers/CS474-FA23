# Deric Shaffer
# CS474 - Programming Assignment 3
# Due Date - Oct. 15th, 2023
# Purpose - makefile for PA3

all: normal shared thread

normal: merge.c
	gcc -o normal merge.c
	
shared: shared.c
	gcc -o shared shared.c
	
thread: thread.c
	gcc thread.c -lpthread -o thread