#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>

using namespace std;

class Job {
public:
    int arrival_time;
    int cores;
    int memory;
    int execution_time;
    int gross_value;

    Job(int arrival_time, int cores, int memory, int execution_time) {
        this->arrival_time = arrival_time;
        this->cores = cores;
        this->memory = memory;
        this->execution_time = execution_time;
        this->gross_value = cores * memory * execution_time;
    }
};

class WorkerNode {
public:
    int cores_available = 24;
    int memory_available = 64;

    bool can_allocate(Job& job) {
        return cores_available >= job.cores && memory_available >= job.memory;
    }

    void allocate(Job& job) {
        cores_available -= job.cores;
        memory_available -= job.memory;
    }

    void deallocate(Job& job) {
        cores_available += job.cores;
        memory_available += job.memory;
    }
};

bool sort_by_duration(const Job &a, const Job &b) {
    return a.execution_time < b.execution_time;
}

bool sort_by_gross_value(const Job &a, const Job &b) {
    return a.gross_value < b.gross_value;
}

void simulate_queue_and_node_policies() {
    vector<Job> jobs;
    for (int i = 0; i < 1000; ++i) {
        int arrival_time = rand() % 24;
        int cores = rand() % 24 + 1;
        int memory = rand() % 64 + 1;
        int execution_time = rand() % 5 + 1;
        jobs.push_back(Job(arrival_time, cores, memory, execution_time));
    }


    vector<WorkerNode> workers(128);

    
    vector<string> queue_policies = {"FCFS", "Smallest Job First", "Shortest Duration"};
    vector<string> node_policies = {"First Fit", "Best Fit", "Worst Fit"};

    ofstream file("utilization_results.csv");
    file << "Queue Policy,Node Policy,CPU Utilization,Memory Utilization\n";

    for (string queue_policy : queue_policies) {
        for (string node_policy : node_policies) {
           
            if (queue_policy == "Smallest Job First") {
                sort(jobs.begin(), jobs.end(), sort_by_gross_value);
            } else if (queue_policy == "Shortest Duration") {
                sort(jobs.begin(), jobs.end(), sort_by_duration);
            }
            
            double cpu_utilization, memory_utilization;
            tie(cpu_utilization, memory_utilization) = run_simulation(jobs, workers, node_policy);

            
            file << queue_policy << "," << node_policy << "," << cpu_utilization << "," << memory_utilization << "\n";
        }
    }

    file.close();
}
 
pair<double, double> run_simulation(vector<Job>& jobs, vector<WorkerNode>& workers, string node_policy) {
    int total_cores = 128 * 24;
    int total_memory = 128 * 64;

    int used_cores = 0;
    int used_memory = 0;
    
    for (Job& job : jobs) {
        if (node_policy == "First Fit") {
            
            for (WorkerNode& worker : workers) {
                if (worker.can_allocate(job)) {
                    worker.allocate(job);
                    used_cores += job.cores;
                    used_memory += job.memory;
                    break;
                }
            }
        } else if (node_policy == "Best Fit") {
           
            WorkerNode* best_node = nullptr;
            int min_remaining_cores = 25, min_remaining_memory = 65;

            for (WorkerNode& worker : workers) {
                if (worker.can_allocate(job)) {
                    int remaining_cores = worker.cores_available - job.cores;
                    int remaining_memory = worker.memory_available - job.memory;
                    if (remaining_cores < min_remaining_cores && remaining_memory < min_remaining_memory) {
                        best_node = &worker;
                        min_remaining_cores = remaining_cores;
                        min_remaining_memory = remaining_memory;
                    }
                }
            }

            if (best_node) {
                best_node->allocate(job);
                used_cores += job.cores;
                used_memory += job.memory;
            }
        } else if (node_policy == "Worst Fit") {
        
            WorkerNode* worst_node = nullptr;
            int max_remaining_cores = -1, max_remaining_memory = -1;

            for (WorkerNode& worker : workers) {
                if (worker.can_allocate(job)) {
                    int remaining_cores = worker.cores_available - job.cores;
                    int remaining_memory = worker.memory_available - job.memory;
                    if (remaining_cores > max_remaining_cores || remaining_memory > max_remaining_memory) {
                        worst_node = &worker;
                        max_remaining_cores = remaining_cores;
                        max_remaining_memory = remaining_memory;
                    }
                }
            }

            if (worst_node) {
                worst_node->allocate(job);
                used_cores += job.cores;
                used_memory += job.memory;
            }
        }
    }

    
    double cpu_utilization = (double(used_cores) / total_cores) * 100;
    double memory_utilization = (double(used_memory) / total_memory) * 100;

    return make_pair(cpu_utilization, memory_utilization);
}

int main() {
    
    simulate_queue_and_node_policies();
    
    cout << "Simulation completed and results saved to CSV." << endl;

    return 0;
}