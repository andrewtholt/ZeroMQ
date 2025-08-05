CC = g++
# CC = gcc
CFLAGS = -Wall -g -I.
LDFLAGS = -lzmq
RM = rm -f

all: requester replier ps_publisher ps_subscriber jsonTest publisher

requester: requester.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

replier: replier.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ps_publisher: ps_publisher.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

ps_subscriber: ps_subscriber.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

jsonTest: jsonTest.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

publisher: publisher.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) requester replier ps_publisher ps_subscriber jsonTest publisher
