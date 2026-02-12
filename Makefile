src = ./src

clay = $(src)/clay
ui = $(src)/ui
lsys = $(src)/lsystem
parser = $(lsys)/parser
turtle = $(src)/turtle
util = $(src)/util
elems = $(ui)/elements

build = ./build
objs = $(build)/objects

mkdirs =  @if [ ! -d $(objs) ]; then mkdir -p $(objs); fi;

aristid: $(objs)/main.o $(objs)/ui.o $(objs)/textbox.o \
	 $(objs)/module.o $(objs)/inputbox.o $(objs)/custom.o \
	 $(objs)/util.o $(objs)/turtle.o $(objs)/lsystem.o $(objs)/lsystem_parser.o \
	 $(objs)/clay_renderer_raylib.o $(objs)/fonts.o $(ui)/move_container.o \
	 $(objs)/vectorbox.o $(objs)/turtlebox.o \
	 $(objs)/lsys_parse_util.o
	$(mkdirs)
	$(CC) -lm -lraylib $(LDFLAGS) $^ -o $(build)/aristid

$(objs)/main.o: $(src)/main.c $(turtle)/turtle.h \
	$(ui)/ui.h $(ui)/custom.h \
	$(clay)/clay_renderer_raylib.h
	$(mkdirs)
	$(CC) $(CFLAGS) -c $(src)/main.c -o $(objs)/main.o

$(objs)/ui.o: $(ui)/ui.c $(ui)/ui.h $(clay)/clay.h \
      $(clay)/clay_renderer_raylib.h \
      $(ui)/keys.h $(ui)/textbox.h \
      $(ui)/turtlebox.h \
      $(ui)/module.h $(ui)/common.h \
      $(ui)/custom.h $(ui)/fonts.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/ui.c -o $(objs)/ui.o

$(objs)/textbox.o: $(ui)/textbox.c $(ui)/textbox.h \
	   $(ui)/inputbox.h $(ui)/module.h \
	   $(turtle)/turtle.h $(lsys)/lsystem.h \
           $(lsys)/lsystem_parser.h \
           $(ui)/common.h $(clay)/clay.h
	$(mkdirs) 
	$(CC) $(CFLAGS)  -c $(ui)/textbox.c -o $(objs)/textbox.o

$(objs)/module.o: $(ui)/module.c $(ui)/module.h \
	  $(turtle)/turtle.h $(lsys)/lsystem.h \
	  $(ui)/common.h $(clay)/clay.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/module.c -o $(objs)/module.o

$(objs)/turtlebox.o: $(elems)/turtlebox.c $(elems)/turtlebox.h \
	    $(ui)/vectorbox.h $(ui)/inputbox.h \
	    $(ui)/common.h $(clay)/clay.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(elems)/turtlebox.c -o $(objs)/turtlebox.o

$(objs)/vectorbox.o: $(ui)/vectorbox.c $(ui)/vectorbox.h \
	    $(ui)/inputbox.h \
	    $(ui)/common.h $(clay)/clay.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/vectorbox.c -o $(objs)/vectorbox.o

$(objs)/inputbox.o: $(ui)/inputbox.c $(ui)/inputbox.h \
	    $(ui)/common.h $(clay)/clay.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/inputbox.c -o $(objs)/inputbox.o

$(objs)/move_container.o: $(ui)/move_container.c $(ui)/move_container.h 
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/move_container.c -o $(objs)/move_container.o

$(objs)/custom.o: $(ui)/custom.c $(ui)/custom.h \
	  $(ui)/textbox.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/custom.c -o $(objs)/custom.o
	 
$(objs)/util.o: $(util)/util.c $(util)/util.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(util)/util.c -o $(objs)/util.o

$(objs)/turtle.o: $(turtle)/turtle.c $(turtle)/turtle.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(turtle)/turtle.c -o $(objs)/turtle.o

$(objs)/fonts.o: $(ui)/fonts.c $(ui)/fonts.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(ui)/fonts.c -o $(objs)/fonts.o

$(objs)/lsystem_parser.o: $(parser)/lsystem_parser.c \
	$(parser)/lsystem_parser.h $(parser)/lsys_parse_util.h \
	$(lsys)/lsystem.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(parser)/lsystem_parser.c -o $(objs)/lsystem_parser.o

$(objs)/lsys_parse_util.o: $(parser)/lsys_parse_util.c \
	$(parser)/lsys_parse_util.h $(lsys)/lsystem.h
	$(CC) $(CFLAGS)  -c $(parser)/lsys_parse_util.c -o $(objs)/lsys_parse_util.o

$(parser)/lsystem_parser.c: $(parser)/lsystem_parser.peg
	packcc $(parser)/lsystem_parser.peg -o $(parser)/lsystem_parser

$(objs)/lsystem.o: $(lsys)/lsystem.c $(lsys)/lsystem.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(lsys)/lsystem.c -o $(objs)/lsystem.o

$(objs)/clay_renderer_raylib.o: $(clay)/clay_renderer_raylib.c \
	                $(clay)/clay_renderer_raylib.h
	$(mkdirs)
	$(CC) $(CFLAGS)  -c $(clay)/clay_renderer_raylib.c -o $(objs)/clay_renderer_raylib.o


.PHONY: clean
clean: 
	rm -rf ./build/

.PHONY: run
run: aristid
	./build/aristid

