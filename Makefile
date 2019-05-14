SOURCE	= structs.cpp maint.cpp bidbfs.cpp grail.cpp thread.cpp strongly.cpp
HEADER  = structs.hpp bidbfs.hpp grail.hpp thread.hpp strongly.hpp
OBJ =  structs.o maint.o bidbfs.o grail.o thread.o strongly.o
OUT	= maint test
CC	= g++
FLAGS = -g -c

all: $(HEADER)  $(OBJS)
	$(CC) $(OBJ) -o maint -lpthread

test: $(OBJS) $(HEADER)
		$(CC) -o test strongly.cpp strcuts.cpp

maint.o: structs.cpp maint.cpp bidbfs.cpp grail.cpp thread.cpp strongly.cpp structs.hpp bidbfs.hpp grail.hpp thread.hpp strongly.hpp
	$(CC) $(FLAGS) maint.cpp

thread.o:structs.cpp bidbfs.cpp grail.cpp thread.cpp strongly.cpp structs.hpp bidbfs.hpp grail.hpp thread.hpp strongly.hpp
	$(CC) $(FLAGS) thread.cpp

bidbfs.o: structs.cpp bidbfs.cpp structs.hpp bidbfs.hpp grail.cpp grail.hpp strognly.hpp strongly.cpp
	$(CC) $(FLAGS) bidbfs.cpp

grail.o: structs.cpp structs.hpp grail.cpp grail.hpp strognly.hpp strongly.cpp
	$(CC) $(FLAGS) grail.cpp

strongly.o: structs.cpp structs.hpp strognly.hpp strongly.cpp
	$(CC) $(FLAGS) strongly.cpp

structs.o: structs.cpp structs.hpp
	$(CC) $(FLAGS) structs.cpp

clean:
	rm -i $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HEADER)
