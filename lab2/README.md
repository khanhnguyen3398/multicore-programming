compilation instructions:

g++ -std=c++11 -o lab2 main.cpp -lpthread
./lab2 -n 50


WorkNode that'll be in the work queue where the threads from thread pool
can pull off of to do work and also pushed to the second queue for main
to update. 

Each node contains as well as the current best set
of coefficients and fitness point so that each thread can continue to work on.
Has functions associated with the work, i.e mutate the polyomials, finding the
fitness coefficient. 

Create a threadpool class that uses DECENTRALIZED scheduling.
Have a work_queue of work node where each thread will listen to
this queue where as soon as there's work it will get the work and
start working. When it is done it will push to a second queue results_queue
where main will listen to and extract results from. 
Every time main extracted the result, it will push a new work node onto the 
work_queue so that the listening threads can pull it out to work on. Main thread
will check to see if the node pushed by thread workers contain a better set
of coefficients, i.e a better fitness score. 

Note: I'm using my thread-safe listener queue implementation from lab2.

Here are the methods in main:

// this function is for every thread to run and receive work, called when thread spawned
static void tester(ThreadSafeListenerQueue<WorkNode>& work_queue, ThreadSafeListenerQueue<WorkNode>& results_queue, int pos);

// this function is used to mutate the coefficients. 
static void mutate(WorkNode *work, int id);
// this funciton is used to compute fitness by calculating the distance between point and line
static void comp_fitness(WorkNode *work);
//this is used to compute y for every data point x of a given polynomial
static double compute_y(WorkNode *work, double x);
//this is used to compute x^power of something
static double compute_power(double x, int power );
//this is used to pring the result
static void print_result(WorkNode current, int degree);
