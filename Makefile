CC = g++
# CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lzmq
RM = rm -f

all: requester replier ps_publisher ps_subscriber

requester: requester.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

replier: replier.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)



ps_publisher: ps_publisher.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ps_subscriber: ps_subscriber.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) requester replier ps_publisher ps_subscriber
