#pragma once
#include <fstream>
#include <string>
#include <shared_mutex>
#include <tool_functions/ThreadPool.h>
#include <boost/heap/fibonacci_heap.hpp>
#include <dgraph_v_of_v/dgraph_v_of_v.h>

#define two_hop_weight_type float

class two_hop_label
{
public:
	int vertex;
	two_hop_weight_type distance;
};

/*common functions shared by PLL and PSL*/
bool compare_two_hop_label_vertex_small_to_large_and_distance_small_to_large(const two_hop_label& i, two_hop_label& j)
{
	return i.vertex == j.vertex ? i.distance < j.distance : i.vertex < j.vertex;
}

bool compare_two_hop_label_vertex_small_to_large(const two_hop_label& i, two_hop_label& j)
{
	return i.vertex < j.vertex;
}

long long int compute_label_bit_size(vector<vector<two_hop_label>>& L_in, vector<vector<two_hop_label>>& L_out) {
	long long int size = 0;
	auto it_end = L_in.end();
	for (auto it = L_in.begin(); it != it_end; it++) {
		size = size + (*it).size();
	}
	it_end = L_out.end();
	for (auto it = L_out.begin(); it != it_end; it++) {
		size = size + (*it).size();
	}
	return size * sizeof(two_hop_label);
}

void label_output_to_file(std::string instance_name, vector<vector<two_hop_label>>& L_in, vector<vector<two_hop_label>>& L_out)
{
	std::ofstream outputFile;
	outputFile.open(instance_name);

	outputFile << "in label" << std::endl;

	int size1 = L_in.size();
	for (int i = 0; i < size1; i++)
	{
		outputFile << i << ":\t";
		int size2 = L_in[i].size();
		for (int j = 0; j < size2; j++)
		{
			outputFile << "(" << L_in[i][j].vertex << "," << L_in[i][j].distance << ") ";
		}
		outputFile << std::endl;
	}

	outputFile << std::endl << std::endl << std::endl << "out label" << std::endl;;

	size1 = L_out.size();
	for (int i = 0; i < size1; i++)
	{
		outputFile << i << ":\t";
		int size2 = L_out[i].size();
		for (int j = 0; j < size2; j++)
		{
			outputFile << "(" << L_out[i][j].vertex << "," << L_out[i][j].distance << ") ";
		}
		outputFile << std::endl;
	}
}



/* struct used for dijkstra extra_min */

struct node_for_dij {
public:
	int vertex;
	two_hop_weight_type priority_value;
};

// < is the max-heap; > is the min heap

bool operator<(node_for_dij const& x, node_for_dij const& y) {
	return x.priority_value > y.priority_value;
}


typedef typename boost::heap::fibonacci_heap<node_for_dij>::handle_type dgraph_heap_pointer;




/*定义全局变量*/
string reach_limit_error_string_MB = "reach limit error MB";
string reach_limit_error_string_time = "reach limit error time";
int max_N_ID_for_mtx_595 = 1e7;
vector<std::shared_mutex> mtx_595(max_N_ID_for_mtx_595); /* multi thread parameters */
std::shared_mutex edge_lock;
queue<int> Qid_595;
vector<vector<dgraph_heap_pointer>> Q_pointers;
vector<vector<two_hop_weight_type>> dist;
vector<vector<two_hop_weight_type>> dist2;
vector<int> increment[2];
vector<int> pos_595[2];
vector<int> pos_2_595[2];
//vector<vector<int>> canonical_repair_removed_hubs_IN, canonical_repair_removed_hubs_OUT;
shared_mutex mtx;
bool no_new_label_PSL;
vector<vector<bool>> dirt;
vector<vector<two_hop_label>> L_temp_in;
vector<vector<two_hop_label>> L_temp_out;
vector<vector<two_hop_label>> L_PSL_temp[2];
auto begin_time_PLL = std::chrono::high_resolution_clock::now();
double max_run_time_nanoseconds_PLL;
long long int max_labal_size_PLL;
long long int labal_size_PLL;
bool this_parallel_PLL_is_running = false;
bool PLL_throw_error = false;
auto begin_time_PSL = std::chrono::high_resolution_clock::now();
double max_run_time_nanoseconds_PSL;
long long int max_labal_size_PSL;
long long int labal_size_PSL;
bool this_parallel_PSL_is_running = false;
bool PSL_throw_error = false;

/*清除全局变量*/
void dgraph_clear_global_values_PLL_PSL() {
	//vector<vector<int>>().swap(canonical_repair_removed_hubs_IN);
	//vector<vector<int>>().swap(canonical_repair_removed_hubs_OUT);
	vector<vector<dgraph_heap_pointer>>().swap(Q_pointers);
	this_parallel_PLL_is_running = false;
	this_parallel_PSL_is_running = false;
	PLL_throw_error = false;
	PSL_throw_error = false;
	queue<int>().swap(Qid_595);
	vector<vector<bool>>().swap(dirt);
	vector<vector<two_hop_label>>().swap(L_temp_in);
	vector<vector<two_hop_label>>().swap(L_temp_out);
	vector<vector<two_hop_weight_type>>().swap(dist);
	vector<vector<two_hop_weight_type>>().swap(dist2);
	for (int k = 0; k < 2; k++) {
		vector<int>().swap(increment[k]);
		vector<int>().swap(pos_595[k]);
		vector<int>().swap(pos_2_595[k]);
		vector<vector<two_hop_label>>().swap(L_PSL_temp[k]);
	}
}






/*class*/

class dgraph_case_info_v1 {
public:
	/*running limits*/
	long long int max_labal_bit_size = 1e12; // 2-hop-label num
	double max_run_time_seconds = 1e12;

	// for canonical repair
	bool use_canonical_repair = false;
	long long int label_size_before_canonical_repair = 0;
	long long int label_size_after_canonical_repair = 0;

	/*labels*/
	vector<vector<two_hop_label>> L_in;
	vector<vector<two_hop_label>> L_out;

	/*indexing times*/
	double time1_PLL_PSL_initialization = 0;
	double time2_PLL_PSL_label_generation = 0;
	double time3_PLL_PSL_label_postprocess = 0;
	double time4_PLL_PSL_label_canonical = 0;
	double time5_PLL_PSL_total = 0;

	/*clear labels*/
	void clear_labels() {
		vector<vector<two_hop_label>>().swap(L_in);
		vector<vector<two_hop_label>>().swap(L_out);
	}

	/*printing*/
	void print_L() {
		cout << "in label" << std::endl;
		int size1 = L_in.size();
		for (int i = 0; i < size1; i++)
		{
			cout << i << ":\t";
			int size2 = L_in[i].size();
			for (int j = 0; j < size2; j++)
			{
				cout << "(" << L_in[i][j].vertex << "," << L_in[i][j].distance << ") ";
			}
			cout << std::endl;
		}
		cout << std::endl << std::endl << std::endl << "out label" << std::endl;;
		size1 = L_out.size();
		for (int i = 0; i < size1; i++)
		{
			cout << i << ":\t";
			int size2 = L_out[i].size();
			for (int j = 0; j < size2; j++)
			{
				cout << "(" << L_out[i][j].vertex << "," << L_out[i][j].distance << ") ";
			}
			cout << std::endl;
		}
	}
};






/*query distance*/

two_hop_weight_type dgraph_v1_extract_shortest_distance(vector<vector<two_hop_label>>& L_in, vector<vector<two_hop_label>>& L_out, int source, int terminal)
{
	if (source == terminal)
	{
		return 0;
	}

	two_hop_weight_type distance = std::numeric_limits<two_hop_weight_type>::max(); // if disconnected, return this large value
	auto vector1_check_pointer = L_out[source].begin();
	auto vector2_check_pointer = L_in[terminal].begin();
	auto pointer_L_s_end = L_out[source].end(), pointer_L_t_end = L_in[terminal].end();
	while (vector1_check_pointer != pointer_L_s_end && vector2_check_pointer != pointer_L_t_end) {
		if (vector1_check_pointer->vertex == vector2_check_pointer->vertex) {
			two_hop_weight_type dis = vector1_check_pointer->distance + vector2_check_pointer->distance;
			if (distance > dis) {
				distance = dis;
			}
			vector1_check_pointer++;
		}
		else if (vector1_check_pointer->vertex > vector2_check_pointer->vertex) {
			vector2_check_pointer++;
		}
		else {
			vector1_check_pointer++;
		}
	}

	return distance;
}





/*canonical_repair*/

two_hop_weight_type dgraph_v1_extract_shortest_distance_for_canonical_repair(int source, int terminal, int in_or_out, vector<two_hop_label>::iterator limit) {

	two_hop_weight_type distance = std::numeric_limits<two_hop_weight_type>::max(); // if disconnected, return this large value
	auto vector1_check_pointer = L_temp_out[source].begin();
	auto vector2_check_pointer = L_temp_in[terminal].begin();
	auto pointer_L_s_end = L_temp_out[source].end(), pointer_L_t_end = L_temp_in[terminal].end();
	if (in_or_out == 1) {
		pointer_L_s_end = limit;
	}
	else {
		pointer_L_t_end = limit;
	}
	while (vector1_check_pointer != pointer_L_s_end && vector2_check_pointer != pointer_L_t_end) {
		if (vector1_check_pointer->vertex == vector2_check_pointer->vertex) {
			two_hop_weight_type dis = vector1_check_pointer->distance + vector2_check_pointer->distance;
			if (distance > dis)
			{
				distance = dis;
			}
			vector1_check_pointer++;
		}
		else if (vector1_check_pointer->vertex > vector2_check_pointer->vertex) {
			vector2_check_pointer++;
		}
		else {
			vector1_check_pointer++;
		}
	}

	return distance;
}

void canonical_repair_in(int target_v, vector<vector<two_hop_label>>* L_final) {
	auto begin = L_temp_in[target_v].begin(), end = L_temp_in[target_v].end();
	for (; begin != end; begin++) {
		int vertex = begin->vertex;
		if (vertex == target_v) {
            (*L_final)[target_v].push_back(*begin);
			continue;
		}
        two_hop_weight_type query_dis = dgraph_v1_extract_shortest_distance_for_canonical_repair(vertex, target_v, 0, begin);

		if (query_dis >= begin->distance + 1e-5) { // 1e-5 is precision
            (*L_final)[target_v].push_back(*begin);
		}
	}
    (*L_final).shrink_to_fit();
}

void canonical_repair_out(int target_v, vector<vector<two_hop_label>>* L_final) {
    auto begin = L_temp_out[target_v].begin(), end = L_temp_out[target_v].end();
    for (; begin != end; begin++) {
        int vertex = begin->vertex;
        if (vertex == target_v) {
            (*L_final)[target_v].push_back(*begin);
            continue;
        }
        two_hop_weight_type query_dis = dgraph_v1_extract_shortest_distance_for_canonical_repair(target_v, vertex, 1, begin);

        if (query_dis >= begin->distance + 1e-5) { // 1e-5 is precision
            (*L_final)[target_v].push_back(*begin);
        }
    }
    (*L_final).shrink_to_fit();
}

/*the following codes of recording removed hubs is slower*/
//void update_L_final(vector<int>& canonical_repair_removed_hubs, vector<two_hop_label>* L_old, vector<two_hop_label>* L_final) {
//	auto begin = L_old->begin(), end = L_old->end();
//	auto begin2 = canonical_repair_removed_hubs.begin(), end2 = canonical_repair_removed_hubs.end();
//	while (begin != end) {
//		if (begin2 == end2) {
//			L_final->insert(L_final->end(), begin, end);
//			break;
//		}
//		if (begin->vertex < *begin2) {
//			L_final->push_back(*begin);
//			begin++;
//		}
//		else if (begin->vertex == *begin2) {
//			begin++;
//			begin2++;
//		}
//	}
//	//vector<two_hop_label>(*L_final).swap(*L_final);
//}
//void canonical_repair_in(int target_v, vector<vector<two_hop_label>>* L_final) {
//	auto begin = L_temp_in[target_v].begin(), end = L_temp_in[target_v].end();
//	for (; begin != end; begin++) {
//		if (begin->vertex == target_v) {
//			continue;
//		}
//		two_hop_weight_type query_dis = dgraph_v1_extract_shortest_distance_for_canonical_repair(begin->vertex, target_v, 0, begin);
//		if (query_dis < begin->distance + 1e-5) { // 1e-5 is precision
//			canonical_repair_removed_hubs_IN[target_v].push_back(begin->vertex);
//		}
//	}
//	if (canonical_repair_removed_hubs_IN[target_v].size()) {
//		update_L_final(canonical_repair_removed_hubs_IN[target_v], &L_temp_in[target_v], &(*L_final)[target_v]);
//	}
//	else {
//		(*L_final)[target_v] = L_temp_in[target_v];
//	}
//}
//void canonical_repair_out(int target_v, vector<vector<two_hop_label>>* L_final) {
//	auto begin = L_temp_out[target_v].begin(), end = L_temp_out[target_v].end();
//	for (; begin != end; begin++) {
//		if (begin->vertex == target_v) {
//			continue;
//		}
//		two_hop_weight_type query_dis = dgraph_v1_extract_shortest_distance_for_canonical_repair(target_v, begin->vertex, 1, begin);
//		if (query_dis < begin->distance + 1e-5) { // 1e-5 is precision
//			canonical_repair_removed_hubs_OUT[target_v].push_back(begin->vertex);
//		}
//	}
//	if (canonical_repair_removed_hubs_OUT[target_v].size()) {
//		update_L_final(canonical_repair_removed_hubs_OUT[target_v], &L_temp_out[target_v], &(*L_final)[target_v]);
//	}
//	else {
//		(*L_final)[target_v] = L_temp_out[target_v];
//	}
//}

void canonical_repair_multi_threads(ThreadPool& pool, std::vector<std::future<int>>& results, vector<vector<two_hop_label>>* L_in_final, vector<vector<two_hop_label>>* L_out_final) {

	int N = L_temp_in.size();
	(*L_in_final).resize(N), (*L_out_final).resize(N);
	//canonical_repair_removed_hubs_IN.resize(N), canonical_repair_removed_hubs_OUT.resize(N);

	for (int target_v = 0; target_v < N; target_v++) {
		results.emplace_back(
			pool.enqueue([target_v, L_in_final, L_out_final] { // pass const type value j to thread; [] can be empty
				canonical_repair_in(target_v, L_in_final);
				canonical_repair_out(target_v, L_out_final);
				return 1; // return to results; the return type must be the same with results
				}));
	}
	for (auto&& result : results)
		result.get(); // all threads finish here
	results.clear();
}
