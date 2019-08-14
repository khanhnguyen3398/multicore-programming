compilation instructions:

g++ -std=c++17 -o lab4 main.cpp -lpthread

./lab4 <number_oftotal threads> <threshold g for determining
completion> <rows> <cols> <genome length>

Threshold has been 100,000 for all my experiments.

What I found is that the larger the maze size, the worse the time will
get and also the worse the score will get. It also means that the
genome size will matter more as well. There's an optimal range of genome size
the matches each particular maze size, that will give the best score.
Genome size doesn't really correspond with the time, it's mostly the maze size.
Shorter genome size for a large maze such as 17x 17 won't give a good score as well
as a good time.There's also a trade-off that I noticed between
getting better scores and getting better time. To get better scores, I found that
I needed to increase the proportion of mixer threads to mutator threads, which in
return slows down the whole program. The proportion that I thought
was the most appropriate is 70% mixer and 30% mutator. When I incrase the
mutator thread, the speed gets better but sometime with worse scores,
especially with bigger mazes. I also found that it's quite peculiar
that more threads doesn't neccessarily mean that the program will run faster. 
When I run with 6 threads, the program actually runs much faster than when running with
8 or 12 threads and the tradeoff between accuracy and speed also subsides. 
There's actually not much of a difference between running at 8 threads and 12 threads but
there's a stark difference when running with 6 threads. Running 6 threads also give 
better scores as well, with a large difference in scores especially with larger mazes. 

