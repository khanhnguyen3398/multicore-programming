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


static double OPTIMAL_FIT = 1.0;
std::vector<double> data_points_x;
std::vector<double> data_points_y;
//std::vector<double> runtime;
bool keep_working = true;
std::chrono::duration<double> max;
std::chrono::duration<double> min;
std::chrono::duration<double> total;

static void tester(ThreadSafeListenerQueue<WorkNode>& work_queue, ThreadSafeListenerQueue<WorkNode>& results_queue, int pos);
static void mutate(WorkNode *work, int id);
static double comp_fitness(WorkNode *work);
static double comp_fitness_res(WorkNode *work);
static double compute_y(WorkNode *work, double x);
static double compute_power(double x, int power );
static void print_result(WorkNode current, int degree, int thread_guesses);



int main(int argc, char **argv)
{
	// get the number of threads from command line
	//srand(time(0));
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); 

	int num_threads = 0;
	int deg = 0;
	int seed = 0;
	char* argument;
	char* argument1;
	char* argument2;
    int c ;
    while( ( c = getopt(argc, argv, "t:n:") ) != -1 ) 
    {
        switch(c)
        {
            case 't':
                if(optarg) 
                	argument = optarg;
                break;
           	case 'n' :
           		if (optarg)
           			argument1 = optarg;

        }
    }
    //std::cout << "argument is : " << argument << std::endl;
    num_threads = std::stoi(argument);
    deg = std::stoi(argument1);
    //long s = std::atol(argument2);
 

	ThreadSafeListenerQueue<WorkNode> work_queue;
	ThreadSafeListenerQueue<WorkNode> results_queue;
	WorkNode current;

	//std::cout << "queue size of work is : " << work_queue.size() << "\n";
	auto seed2 = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	auto double_rand = std::bind(std::uniform_real_distribution<double>(-5,5), std::mt19937(seed2));
	auto coeff_rand = std::bind(std::uniform_real_distribution<double>(-5,5), std::mt19937(seed2));
	//auto int_rand = std::bind(std::uniform_int_distribution<double>(0,1000), std::mt19937(seed));

  	double co;
  	for (int a = 0; a <= deg ; a++) {
  		co = coeff_rand();
  		//std::cout << "coeff : " << co << "\n";
  		current.coefficients.push_back(co);
  	}

  	double x;
  	double y;

  	for (int i = 0; i <= deg ; i++) {
  		x = double_rand();
  		y = double_rand();
  		//std::cout << "x : " << x << std::endl;
  		//std::cout << "y : " << y << std::endl;
  		data_points_x.push_back(x);
  		data_points_y.push_back(y);
  	}

  	/*	data_points_x.push_back(3.04244);
  		data_points_y.push_back(-1.22392);

  			data_points_x.push_back(-2.27708);
  	//	data_points_y.push_back(4.03803);

  				data_points_x.push_back(-1.05765);
  		data_points_y.push_back(-0.363712);
  		
  			data_points_x.push_back(-4.57277);
  	*/	//data_points_y.push_back(3.91337);


  	double fit = 0;
  	fit = comp_fitness(&current);
  	current.fitness_score = fit;

  	int thread_guesses = 0;

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

	for (int b = 0; b < num_threads; b++) {
		threads.push_back(std::thread(tester, std::ref(work_queue), std::ref(results_queue), b));
	}
	int num_queue = num_threads;
	WorkNode element;

	// main thread listens onto the queue for any sum pushed onto queue
	// by each threads spawned above
	while (fabs(current.fitness_score) > OPTIMAL_FIT) {
		results_queue.listen(element);
		thread_guesses++;
		if (thread_guesses == 1 ) {
			max = element.time;
			min = element.time;
		} 
		if (element.time > max) {
			max = element.time;
		} 
		if (element.time < min) {
			min = element.time;
		}
		total += element.time;
		//std::cout << " listened successfully : \n";
		//std::cout << "current fitness : " << current.fitness_score << "\n";
		//std::cout << "element fitness : " << element.fitness_score << "\n";
		
		if (fabs(element.fitness_score) < fabs(current.fitness_score) ) {
			//std::cout << "thread found better \n" ;
			//std::cout << "current fitness : " << current.fitness_score << "\n";
			//std::cout << "element fitness : " << element.fitness_score << "\n";
			current.fitness_score = element.fitness_score;
			//std::cout<< "Current best polynomial in order of power = 0 to power = " << deg << " : \n";
			//copy over
			for (int k = 0; k <= deg; k++) {
				current.coefficients.at(k) = element.coefficients.at(k);
				//std::cout << current.coefficients.at(k) << " ";
			}
			//std::cout << "\n" << std::endl;
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
		print_result(current, deg, thread_guesses);

			//keep_working = false;

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now(); 
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);


	std::cout << "Maximum time per thread is: " << max.count() << "seconds" << std::endl;
	std::cout << "Minimum time per thread is: " << min.count() << "seconds" << std::endl;
	std::cout << "Mean time per thread is : " << total.count() / thread_guesses << "seconds " << std::endl;
	std::cout << "Total time taken is: " << time_span.count() << " seconds" << std::endl;
 
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
	double time = 0;
	double current_score = 0;
	double fit = 0;
	while (keep_working) {
		WorkNode work;
		work_queue.listen(work);
		//std::cout << "got work " << std::endl;
		mutate(&work, id);
		//print_result(work, 2);
		results_queue.push(work);
	}
	return;
}

static void mutate(WorkNode *work, int id)
{
	// each thread determines to see its own best before pushing to main, keep changing to see which direction for
	// which coefficient would owkr to lower fitness, maybe record that in the node
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); 
  	double change = 0 ;

  	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  	//std::cout << "work->fitness_score" << work->fitness_score << std::endl;
	auto real_rand1 = std::bind(std::uniform_real_distribution<double>(-1*log(work->fitness_score),log(work->fitness_score)), std::mt19937(seed));
	auto real_rand2 = std::bind(std::uniform_real_distribution<double>(-2.5,1.5), std::mt19937(seed));
	auto real_rand3 = std::bind(std::uniform_real_distribution<int>(-1, 1), std::mt19937(seed));

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

  	double current_fit = work->fitness_score;
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
  		//auto real_rand1 = std::bind(std::uniform_real_distribution<double>(-1*log(work->fitness_score),log(work->fitness_score)), std::mt19937(seed ));
  			change = real_rand1();

  			/*std::cout << "change " << change << std::endl;
  			if (isnan(change)) {
  				std::cout << "change is nan " << std::endl;
  			}
  			if (abs(work->coefficients.at(id)) < 0.0000005) {
  				change = real_rand1();
  				work->coefficients.at(id) = work->coefficients.at(id) + change;
  			}
  			if (work->fitness_score > 10 ) {
  				work->coefficients.at(id) = work->coefficients.at(id) * change;
  			} else {
  				work->coefficients.at(id) = work->coefficients.at(id) + change;
  			}*/
  			double temp_fit = 0;
  			double temp_fit2 = 0;

  		if (work->fitness_score > 20 ) {
  			for (int i = size - 1; i >= 0 ; i--) {
  				work->coefficients.at(i) = work->coefficients.at(i) + change;
  				temp_fit = comp_fitness(work);
  				work->coefficients.at(i) = work->coefficients.at(i) - change;
  				temp_fit2 = comp_fitness(work);
  				if ((temp_fit > temp_fit2) && (temp_fit2 < work->fitness_score))  {
  					work->fitness_score = temp_fit2;
  					break;
  				} else if ((temp_fit < temp_fit2) && (temp_fit < work->fitness_score)) {
  					work->coefficients.at(i) = work->coefficients.at(i) + change;
  					work->fitness_score = temp_fit;
  					break;
  				} 
  			}
  		} else {
  			for (int j = 0; j < size; j++) {
  				work->coefficients.at(j) = work->coefficients.at(j) + change;
  				temp_fit = comp_fitness(work);
  				work->coefficients.at(j) = work->coefficients.at(j) - change;
  				temp_fit2 = comp_fitness(work);
  				if ((temp_fit > temp_fit2) && (temp_fit2 < work->fitness_score))  {
  					work->fitness_score = temp_fit2;
  					break;
  				} else if ((temp_fit < temp_fit2) && (temp_fit < work->fitness_score)) {
  					work->coefficients.at(j) = work->coefficients.at(j) + change;
  					work->fitness_score = temp_fit;
  					break;
  				} 
  			}

  		}

  			/*if (fitness_score >= 30) {
  				change = fitness_score / 3;
  			} else if (fitness_score >= 20 && fitness_score < 30) {
  				change = fitness_score / 2;
  			} 
  			*/
  	//}
  			//std::cout << "coefficient after : " << work->coefficients.at(i) << std::endl;
  		//}
 
  	//}

  	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now(); 
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	work->time = time_span;
  	return;


}

static double comp_fitness(WorkNode *work)
{
	int size = data_points_x.size();
	double y = 0;
	std::vector<double> actual_y;
	for (int i = 0; i < size; i++) {
		if (isnan(data_points_x.at(i))) std::cout << "data pts x is nan \n";
		y = compute_y(work, data_points_x.at(i));
		actual_y.push_back(y);
	}
	double sum = 0;
	double diff = 0;
	for (int j = 0; j < size; j++) {
		if (isnan(actual_y.at(j))) std::cout << "calculated y is nan \n";
		if (isnan(data_points_y.at(j))) std::cout << "data pts y is nan \n";
		diff = actual_y.at(j) - data_points_y.at(j);
		//std::cout << "diff is " << diff << "\n";
		if (isnan(diff)) std::cout << "diff is nan \n";
		sum += compute_power(diff,2);
	}
	//std::cout << "sum is  :" << sum << std::endl;

	return sum;
}

static double comp_fitness_res(WorkNode *work)
{
	int size = data_points_x.size();
	double y = 0;
	std::vector<double> actual_y;
	for (int i = 0; i < size; i++) {
		if (isnan(data_points_x.at(i))) std::cout << "data pts x is nan \n";
		y = compute_y(work, data_points_x.at(i));
		actual_y.push_back(y);
	}
	double sum = 0;
	double diff = 0;
	for (int j = 0; j < size; j++) {
		if (isnan(actual_y.at(j))) std::cout << "calculated y is nan \n";
		if (isnan(data_points_y.at(j))) std::cout << "data pts y is nan \n";
		diff = actual_y.at(j) - data_points_y.at(j);
		if (isnan(diff)) std::cout << "diff is nan \n";
		sum += fabs(diff);
	}
	//std::cout << "sum is  :" << sum << std::endl;

	return sum;
}

static double compute_y(WorkNode *work, double x)
{
	int degree = work->coefficients.size();
	double coeff;
	double y = 0;
	double a = 0;
	for (int i = 0; i < degree; i++) {
		//std::cout << "i in compute y is : " << i << "\n";
		coeff = work->coefficients.at(i);
		if (isnan(coeff)) std::cout << "Coeff is nan \n";

		a =  coeff * pow(x, i);
		y += a;
		if (isnan(y)) std::cout << "compute y is nan \n";

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
	if (isnan(total)) std::cout << "compute power x is nan \n";
	return total;
}




static void print_result(WorkNode current, int degree, int thread_guesses)
{
	//std::cout << "Printing resulting polyomial of degree " << degree << " in order of power = 0 to power = " << degree << "\n";
	//std::cout << "fitness score is: " << current.fitness_score << "\n";
	std::cout << "Data points are :" << "\n";

	for (int j = 0; j <= degree ; j++) {
		std::cout << "(" << data_points_x.at(j) << "," << data_points_y.at(j) << ") \n";
	}

	std::cout << "Printing resulting polyomial of degree " << degree << " in order of power = 0 to power = " << degree << "\n";
	int size = current.coefficients.size();
	for (int i = 0 ; i < size - 1; i++) {
		std::cout << current.coefficients.at(i) << "x^" << i << "+ ";
	}
	std::cout << current.coefficients.at(size - 1) << "x^" << size-1 << "\n";
	comp_fitness_res(&current);
	std::cout << "The number of guesses is: " << thread_guesses << "\n";
}
