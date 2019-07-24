#ifndef WORKNODE_H
#define WORKNODE_H
#include <vector>

class WorkNode {
	public:
		std::vector<double> coefficients;
		double fitness_score;
		int random_seed;
		bool stop_work;
};

#endif