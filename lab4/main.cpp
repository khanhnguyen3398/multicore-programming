#include <iostream>
#include <ctime>  
#include <cstdlib>
#include "getopt.h"
#include <vector>
#include <random>
#include <thread>
#include "ThreadSafeListenerQueue.hpp"
#include "ThreadSafeKVStore.hpp"
#include "maze.cpp"

static void mixer( ThreadSafeKVStore<int, std::vector<int> >& hashmap, ThreadSafeListenerQueue<std::vector<int>>& queue, int num_threads, int id);
static void mutator(Maze problem,ThreadSafeKVStore<int, std::vector<int> >& hashmap, ThreadSafeListenerQueue<std::vector<int>>& queue, int num_threads, int id);
static int comp_fitness(Maze problem, std::vector<int> gen);
static void display(Maze problem, std::vector<int> gen);
int genome_length;
int threshold;
uint64_t futility_cnt;


std::mutex fut_mut;
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

int main(int argc, char **argv)
{
	// get the number of threads from command line
	int num_threads;
    /*	char* argument;
        int c ;
        while( ( c = getopt (argc, argv, "n") ) != -1 ) 
        {
            switch(c)
            {
                case 'n':
                    if(optarg) 
                    	argument = optarg;
                    break;
            }
        }
        */
    num_threads = atoi(argv[1]);
    threshold = atoi(argv[2]);
    int row = atoi(argv[3]);
    int col = atoi(argv[4]);
    genome_length = atoi(argv[5]);
    int mix = num_threads * 0.7;
    int mut = num_threads * 0.3;

    std::srand(std::time(0));   

    Maze m(row, col);

    std::unique_lock<std::mutex> lock(fut_mut);
    futility_cnt = 0;
    lock.unlock();

	ThreadSafeKVStore<int, std::vector<int>> population;
	ThreadSafeListenerQueue<std::vector<int>> offspring_queue;
    std::vector<std::thread> mix_threads;
    std::vector<std::thread> mutator_threads; 

    srand(time(0));
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,4);
    int fit = 0;


    for (int k = 0; k < num_threads * 4; k++) {


        std::vector<int> gen;
        for (int l = 0; l < genome_length; l++) {

            gen.push_back(distribution(generator));
        }
        fit = comp_fitness(m ,gen);

        population.insert(fit, gen);

    }


    for (int i = 0; i < mix ; i++) {


        mix_threads.push_back(std::thread(mixer, std::ref(population), std::ref(offspring_queue), num_threads, i));

    }
    for (int j = 0;  j < mut; j++) {

                                //std::cout << "spawn mjut" << futility_cnt << "\n";

        mutator_threads.push_back(std::thread(mutator, m, std::ref(population), std::ref(offspring_queue), num_threads, j));
    }

    for (int a = 0; a< num_threads; a++) {
        mix_threads.at(a).join();
    }

}

static void mixer(ThreadSafeKVStore<int, std::vector<int> >& hashmap, ThreadSafeListenerQueue<std::vector<int>>& queue, int num_threads, int id) {
    
            
    srand(id);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0, 8);

    while (true ){

        int row1 = distribution(generator);
        int row2 = distribution(generator);

        std::vector<int> gen1 = hashmap[row1];
        std::vector<int> gen2 = hashmap[row2];
        std::vector<int> result;
   
        std::uniform_int_distribution<int> distribution1(1,genome_length - 2);
        int slice = distribution1(generator);

        for (int i = 0; i < slice ; i++ ) {
            if (gen1.empty()) {
            }
            result.push_back(gen1.at(i));
        }

        for (int j = slice; j < genome_length; j++) {
            result.push_back(gen2.at(j));
        }
        queue.push(result);
    }

}

static void mutator(Maze problem, ThreadSafeKVStore<int, std::vector<int>> & hashmap, ThreadSafeListenerQueue<std::vector<int>>& queue, int num_threads, int id) {

    while (true) {
        int cur_best = hashmap.first_score();

        std::vector<int> gen;
        queue.listen(gen);
                /*std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now(); 
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        auto rand_pos = std::bind(std::uniform_real_distribution<int>(1, 10), std::mt19937(seed));
        auto mutate_pos = std::bind(std::uniform_real_distribution<int>(0, genome_length), std::mt19937(seed));
        auto mutate_val = std::bind(std::uniform_real_distribution<int>(0, 4), std::mt19937(seed));
        */

            srand(id);
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution1(1,10);
                std::uniform_int_distribution<int> distribution2(0,4);
                        std::uniform_int_distribution<int> distribution3(0,genome_length -1);

                                    //std::cout << "mutate \n";

        int work = distribution1(generator);
        int pos = distribution3(generator);
        int val = distribution2(generator);
        //int val2 = distribution2(generator);
        //int pos1 = distribution3(generator);

        if (work > 6) {
                       // std::cout << "seg \n";
            gen[pos] = val;
            //gen[pos1] = val2;
                       // std::cout << "segfault \n";
        }

        int mutated_score = comp_fitness(problem, gen);

        hashmap.insert(mutated_score, gen);

        if (hashmap.size() > num_threads * 4) {
                       // std::cout << "trunc \n";
            hashmap.truncate(num_threads*4 - 1);
                       //std::cout << "truncate \n";
        }

        int new_best = hashmap.first_score();

        std::unique_lock<std::mutex> lock(fut_mut);
        if (cur_best > new_best) {
            futility_cnt = 0;
        } else {
            futility_cnt++;
        }
        if (futility_cnt > threshold) {
            display(problem, hashmap.first_elem());
            std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            std::cout << "Total time taken is: " << time_span.count() << " seconds" << std::endl;

            exit(0);
        }
        lock.unlock();
    }



}

static int comp_fitness(Maze problem, std::vector<int> gen) {
    int row = problem.getStart().row;
    int col = problem.getStart().col;
    int hits = 0;
    int val = 0;
    int old_row = row;
    int old_col = col;
    bool wall = 0;

    for (int i = 0; i < genome_length; i++) {
        
        val = gen[i];
        if( val == 1) {
            row--;
        }
        if (val == 2) {
            row++;
        }
        if (val == 3) {
            col--;
        }
        if (val == 4) {
            col++;
        }
        wall = problem.get(row, col);
        if (wall) {
            hits++;
            row = old_row;
            col = old_col;
        } else{
            old_row = row;
            old_col = col;
        }
    }

    int a =problem.getFinish().row - row;
    int b = problem.getFinish().col - col;

    int distance = abs(a) + std::abs(b);
    int fitness = 2 * distance + hits;

    return fitness;

}

static void display(Maze problem, std::vector<int> gen) 
{
    std::cout << "Finding the best path through this maze : \n";
    std::cout << "Start at (" << problem.getStart().row << ", " << problem.getStart().col << ")" << std::endl;
    std::cout << "Finish at (" << problem.getFinish().row << ", " << problem.getFinish().col << ")" << std::endl;
    std::cout << problem;
    int size = gen.size();

    std::cout << "Best path with fitness score of : " << comp_fitness(problem, gen) << "\n";
    for (int i = 0; i < size ; i++) {
        std::cout << gen.at(i) << " ";
    }
    std::cout << "\n";


}

