threaded_sum: threaded_sum.c
	gcc -Wall -Wextra -std=c99 threaded_sum.c -o threaded_sum -pthread

clean:
	rm -f threaded_sum-o threaded_sum