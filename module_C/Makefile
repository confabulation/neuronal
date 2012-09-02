MAIN=vector.c
EXEC=$(patsubst %.c, %, $(MAIN))
SRCS=$(shell ls *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
LIBS= -lrt
HEADERS=$(shell ls *.h)


.PHONY: all
all: $(EXEC)


$(EXEC): $(OBJS) 
	$(CC) $(OBJS) $(LIBS) -o $(EXEC)

%.o: %.c
	$(CC) -c -Wall -Werror -std=gnu99 -pedantic -DCOMPILE_MAIN_VECTOR_C $<

.PHONY: run
run : $(EXEC)
	./$<

.PHONY: clean
clean:
	rm $(OBJS) || true


.PHONY: fullclean
fullclean: clean
	rm $(EXEC)

# force to recompile when a header is modified
$(OBJS): $(HEADERS)


