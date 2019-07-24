#include <iostream>
#include <ctime>  
#include <cstdlib>
#include <thread>
#include "WorkNode.h"
#include "getopt.h"
#include <vector>
#include <random>
#include "ThreadSafeListenerQueue.hpp"
#include <chrono>
#include <functional>


static double OPTIMAL_FIT = 5.0;
std::vector<double> data_points_x;
std::vector<double> data_points_y;
bool keep_working = true;

static void tester(ThreadSafeListenerQueue<WorkNode>& work_queue, ThreadSafeListenerQueue<WorkNode>& results_queue, int pos);
static void mutate(WorkNode *work, int id);
static void comp_fitness(WorkNode *work);
static double compute_y(WorkNode *work, double x);
static double compute_power(double x, int power );
static void print_result(WorkNode current, int degree);



int main(int argc, char **argv)
{
	// get the number of threads from command line
	//srand(time(0));
	int num_threads;
	int deg = 3;
	char* argument;
    int c ;
    while( ( c = getopt (argc, argv, "n:") ) != -1 ) 
    {
        switch(c)
        {
            case 'n':
                if(optarg) 
                	argument = optarg;
                break;
        }
    }
    //std::cout << "argument is : " << argument << std::endl;
    num_threads = std::stoi(argument);

	ThreadSafeListenerQueue<WorkNode> work_queue;
	ThreadSafeListenerQueue<WorkNode> results_queue;
	WorkNode current;

	//std::cout << "queue size of work is : " << work_queue.size() << "\n";
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	auto double_rand = std::bind(std::uniform_real_distribution<double>(-5,5), std::mt19937(seed));
	auto coeff_rand = std::bind(std::uniform_real_distribution<double>(1,5), std::mt19937(seed));
	//auto int_rand = std::bind(std::uniform_int_distribution<double>(0,1000), std::mt19937(seed));

  	double co;
  	for (int a = 0; a <= deg ; a++) {
  		co = coeff_rand();
  		std::cout << "coeff : " << co << "\n";
  		current.coefficients.push_back(co);
  	}

  	double x;
  	double y;

  	for (int i = 0; i <= deg ; i++) {
  		x = double_rand();
  		y = double_rand();
  		std::cout << "x : " << x << std::endl;
  		std::cout << "y : " << y << std::endl;
  		data_points_x.push_back(x);
  		data_points_y.push_back(y);
  	}

  	comp_fitness(&current);


	// spawn the threads and pass the and both queue into those threads
	// each thread calls tester function 
	std::vector<std::thread> threads;
	for (int j = 0; j < num_threads; j++) {
		//std::cout << "spawning threads total thread is " << num_threads << "\n";
		WorkNode work;
		work.fitness_score = current.fitness_score;
		work.coefficients = current.coefficients;
		//work.random_seed = int_rand();
		work_queue.push(work);
	}
	auto int_rand = std::bind(std::uniform_int_distribution<int>(0,deg), std::mt19937(seed));
	int random_seed = 0;
	int pos = 0;
	for (int b = 0; b < num_threads; b++) {
		pos = int_rand();
		threads.push_back(std::thread(tester, std::ref(work_queue), std::ref(results_queue), pos));
	}
	int num_queue = num_threads;
	WorkNode element;

	// main thread listens onto the queue for any sum pushed onto queue
	// by each threads spawned above
	while (fabs(current.fitness_score) > OPTIMAL_FIT) {
		results_queue.listen(element);
		//std::cout << " listened successfully : \n";
		//std::cout << "current fitness : " << current.fitness_score << "\n";
		//std::cout << "element fitness : " << element.fitness_score << "\n";

		if (fabs(element.fitness_score) < fabs(current.fitness_score) ) {
			std::cout << "thread found better \n" ;
			std::cout << "current fitness : " << current.fitness_score << "\n";
			std::cout << "element fitness : " << element.fitness_score << "\n";
			current.fitness_score = element.fitness_score;
			std::cout<< "Current best polynomial in order of power = 0 to power = " << deg << " : \n";
			//copy over
			for (int k = 0; k <= deg; k++) {
				current.coefficients.at(k) = element.coefficients.at(k);
				std::cout << current.coefficients.at(k) << " ";
			}
			std::cout << "\n" << std::endl;
		}
		//std::cout << "creating new work: \n ";
		WorkNode new_work;
		//new_work.random_seed = int_rand();
		new_work.fitness_score = current.fitness_score;
		//element.fitness_score = current.fitness_score;
		for (int l = 0; l <= deg; l++) {
			//std::cout << element.coefficients.at(l) << " ";
			//std::cout << "copying coeff into current l is " << l <<  std::endl;
			new_work.coefficients.push_back(current.coefficients.at(l));
			//element.coefficients.push_back(current.coefficients.at(l));
		}
		//std::cout << "\n";
		//std::cout << "pushing work " << std::endl;
		work_queue.push(element);

		//std::cout << "work queue size after pushing " << work_queue.size() << "\n";
	}
	//std::cout << "while loop done \n";
		print_result(current, deg);

			//keep_working = false;

	exit(0);
	// join the threads
	//for (int j = 0; j< num_threads; j++) {
		//threads.at(j).join();
	//}


	// output the best coefficent in the form of a polyomial as well
	// as the random points
	// output the fitness score




}


static void tester(ThreadSafeListenerQueue<WorkNode>& work_queue, ThreadSafeListenerQueue<WorkNode>& results_queue, int id)
{
	// every thread has a different seed for random generator
	double current_score;
	while ( keep_working ) {
		WorkNode work;
		work_queue.listen(work);
		//std::cout << "got work " << std::endl;
		mutate(&work, id);
		comp_fitness(&work);
		//print_result(work, 2);
		results_queue.push(work);
	}
	return;
}

static void mutate(WorkNode *work, int id)
{

  	double change;

  	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	auto real_rand1 = std::bind(std::uniform_real_distribution<double>(0,log(work->fitness_score)), std::mt19937(seed));
	auto real_rand2 = std::bind(std::uniform_real_distribution<double>(-2.5,1.5), std::mt19937(seed));
	auto real_rand3 = std::bind(std::uniform_real_distribution<double>(-1, 1), std::mt19937(seed));

	//if (work->fitness_score > 1000) {
		//change = real_rand2(); 
	//} else {

	//if (work->fitness_score > 30) {
		//change = real_rand3();
	//}
	//}
  	//std::cout << "pos for change i s " << pos_change << std::endl;
  	//std::cout << "pos" << pos << std::endl;
  	//std::cout << "thread id : " << id << std::endl;

  	//std::cout << "change: " << change << std::endl;
  	//int pos = int_rand();
  	int size = work->coefficients.size();


  	//if (work->fitness_score < 5) {
  		//change = real_rand2();
  		//work->coefficients.at(0) = work->coefficients.at(0) + change;
  	//} else if (work->fitness_score < 3) {
  		//change = real_rand3();
  		//work->coefficients.at(0) = work->coefficients.at(0) + change;
  	//} else {
  			//std::cout << "coefficient before : " << work->coefficients.at(i) << std::endl;
  		//if (work->fitness_score < 7) {
  			//change = real_rand2();
  			//work->coefficients.at(pos) = work->coefficients.at(pos) * change;
  		//} else {
  	//for (int i = 0; i < work->coefficients.size(); i++) {
  			//std::cout << "change " << change << std::endl;
  			change = real_rand2();
  			//std::cout << "change " << change << std::endl;
  			if (abs(work->coefficients.at(id)) < 0.0000005) {
  				change = real_rand1();
  				work->coefficients.at(id) = work->coefficients.at(id) + change;
  			}
  			if (work->fitness_score > 10 ) {
  				work->coefficients.at(id) = work->coefficients.at(id) * change;
  			} else {
  				work->coefficients.at(id) = work->coefficients.at(id) + change;
  			}
  	//}
  			//std::cout << "coefficient after : " << work->coefficients.at(i) << std::endl;
  		//}
 
  	//}


  	return;


}

static void comp_fitness(WorkNode *work)
{
	int size = data_points_x.size();
	std::vector<double> actual_y;
	for (int i = 0; i < size; i++) {
		actual_y.push_back(compute_y(work, data_points_x.at(i)));
	}
	double sum = 0;
	double diff = 0;
	for (int j = 0; j < size; j++) {
		diff = actual_y.at(j) - data_points_y.at(j);
		//std::cout << "difference between x and y is : " << diff << std::endl;
		sum += fabs(diff);
	}
	//std::cout << "sum is  :" << sum << std::endl;
	work->fitness_score = sum;
}

static double compute_y(WorkNode *work, double x)
{
	int degree = work->coefficients.size();
	double coeff;
	double y = 0;
	for (int i = 0; i < degree; i++) {
		//std::cout << "i in compute y is : " << i << "\n";
		coeff = work->coefficients.at(i);
		y += (coeff * compute_power(x, i));
	}
	//y = y*y;
	return y;
}

static double compute_power(double x, int power ) 
{
	double total = x;
	for (int i = 0; i < power - 1; i++) {
		total *= x;
	}
	return total;
}




static void print_result(WorkNode current, int degree)
{
	std::cout << "Printing resulting polyomial of degree " << degree << " in order of power = 0 to power = " << degree << "\n";
	std::cout << "fitness score is: " << current.fitness_score << "\n";
	std::cout << "Data points are :" << "\n";

	for (int j = 0; j <= degree ; j++) {
		std::cout << "(" << data_points_x.at(j) << "," << data_points_y.at(j) << ") \n";
	}

	int size = current.coefficients.size();
	for (int i = 0 ; i < size - 1; i++) {
		std::cout << current.coefficients.at(i) << ", ";
	}
	std::cout << current.coefficients.at(size - 1) << "\n";
}
