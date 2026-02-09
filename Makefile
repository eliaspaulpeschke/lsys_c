src = ./src

clay = $(src)/clay
ui = $(src)/ui
lsys = $(src)/lsystem
turtle = $(src)/turtle
util = $(src)/util

aristid: main.o ui.o textbox.o \
	 module.o inputbox.o custom.o \
	 util.o turtle.o lsystem.o lsystem_parser.o \
	 clay_renderer_raylib.o
	$(CC) -lm -lraylib main.o ui.o textbox.o module.o inputbox.o custom.o util.o turtle.o lsystem.o lsystem_parser.o clay_renderer_raylib.o -o aristid

main.o: $(src)/main.c $(turtle)/turtle.h \
	$(ui)/ui.h $(ui)/custom.h \
	$(clay)/clay_renderer_raylib.h
	$(CC) -c $(src)/main.c

ui.o: $(ui)/ui.c $(ui)/ui.h $(clay)/clay.h \
      $(clay)/clay_renderer_raylib.h \
      $(ui)/keys.h $(ui)/textbox.h \
      $(ui)/module.h $(ui)/common.h \
      $(ui)/custom.h 
	$(CC) -c $(ui)/ui.c

textbox.o: $(ui)/textbox.c $(ui)/textbox.h \
	   $(ui)/inputbox.h $(ui)/module.h \
	   $(turtle)/turtle.h $(lsys)/lsystem.h \
           $(lsys)/lsystem_parser.h \
           $(ui)/common.h $(clay)/clay.h 
	$(CC) -c $(ui)/textbox.c

module.o: $(ui)/module.c $(ui)/module.h \
	  $(turtle)/turtle.h $(lsys)/lsystem.h \
	  $(ui)/common.h $(clay)/clay.h
	$(CC) -c $(ui)/module.c

inputbox.o: $(ui)/inputbox.c $(ui)/inputbox.h \
	    $(ui)/common.h $(clay)/clay.h
	$(CC) -c $(ui)/inputbox.c

custom.o: $(ui)/custom.c $(ui)/custom.h \
	  $(ui)/textbox.h
	$(CC) -c $(ui)/custom.c

util.o: $(util)/util.c $(util)/util.h
	$(CC) -c $(util)/util.c

turtle.o: $(turtle)/turtle.c $(turtle)/turtle.h
	$(CC) -c $(turtle)/turtle.c

lsystem_parser.o: $(lsys)/lsystem_parser.c \
	          $(lsys)/lsystem_parser.h \
		  $(lsys)/lsystem.h \
		  $(util)/util.h
	$(CC) -c $(lsys)/lsystem_parser.c

lsystem.o: $(lsys)/lsystem.c $(lsys)/lsystem.h
	$(CC) -c $(lsys)/lsystem.c

clay_renderer_raylib.o: $(clay)/clay_renderer_raylib.c \
	                $(clay)/clay_renderer_raylib.h
	$(CC) -c $(clay)/clay_renderer_raylib.c
