#pragma once

#include <build_in_progress/HL/dynamic/graph_hash_of_mixed_weighted_PLL_dynamic.h>

void ProDecrease(graph_hash_of_mixed_weighted& instance_graph, vector<vector<two_hop_label_v1>>& L, PPR_type& PPR,
	std::vector<affected_label>& CL_curr, std::vector<affected_label>& CL_next) {

	for (auto it : CL_curr) {
		int v = it.first, u = it.second;
		auto neis = instance_graph.adj_v_and_ec(v);
		for (auto nei : neis) {
			int vnei = nei.first;
			weightTYPE dnew = it.dis + nei.second;
			if (u < vnei) {
				auto query_result = Query2(vnei, u); // query_result is {distance, common hub}
				if (query_result.first > dnew) {
					insert_sorted_two_hop_label(L[vnei], u, dnew);
					CL_next.push_back(affected_label(vnei, u, dnew));
				}
				else {
					auto search_result = search_sorted_two_hop_label2(L[vnei], u);
					if (search_result.first != MAX_VALUE && search_result.first > dnew) {
						L[vnei][search_result.second].distance = dnew;
						CL_next.push_back(affected_label(vnei, u, dnew));
					}
					if (query_result.second != u) {
						PPR_insert(PPR, vnei, query_result.second, u);
					}
					if (query_result.second != vnei) {
						PPR_insert(PPR, u, query_result.second, vnei);
					}
				}
			}
		}
	}
}

void ProDecreasep(graph_hash_of_mixed_weighted* instance_graph, vector<vector<two_hop_label_v1>>* L, PPR_type* PPR,
	std::vector<affected_label>& CL_curr, std::vector<affected_label>* CL_next, ThreadPool& pool_dynamic, std::vector<std::future<int>>& results_dynamic) {

	for (auto it : CL_curr) {
		results_dynamic.emplace_back(pool_dynamic.enqueue([it, L, PPR, CL_next, instance_graph] {

			int v = it.first, u = it.second;
			auto neis = instance_graph->adj_v_and_ec(v);
			for (auto nei : neis) {
				int vnei = nei.first;
				weightTYPE dnew = it.dis + nei.second;
				if (u < vnei) {
					mtx_595[vnei].lock(), mtx_595[u].lock();
					auto query_result = graph_hash_of_mixed_weighted_two_hop_v1_extract_distance_no_reduc2(*L, vnei, u); // query_result is {distance, common hub}
					mtx_595[vnei].unlock(), mtx_595[u].unlock();
					if (query_result.first > dnew) {
						mtx_595[vnei].lock();
						insert_sorted_two_hop_label((*L)[vnei], u, dnew);
						mtx_595[vnei].unlock();
						mtx_595_1.lock();
						CL_next->push_back(affected_label(vnei, u, dnew));
						mtx_595_1.unlock();
					}
					else {
						mtx_595[vnei].lock();
						auto search_result = search_sorted_two_hop_label2((*L)[vnei], u);
						mtx_595[vnei].unlock();
						if (search_result.first != MAX_VALUE && search_result.first > dnew) {
							mtx_595[vnei].lock();
							(*L)[vnei][search_result.second].distance = dnew;
							mtx_595[vnei].unlock();
							mtx_595_1.lock();
							CL_next->push_back(affected_label(vnei, u, dnew));
							mtx_595_1.unlock();
						}
						if (query_result.second != u) {
							mtx_5952[vnei].lock();
							PPR_insert(*PPR, vnei, query_result.second, u);
							mtx_5952[vnei].unlock();
						}
						if (query_result.second != vnei) {
							mtx_5952[u].lock();
							PPR_insert(*PPR, u, query_result.second, vnei);
							mtx_5952[u].unlock();
						}
					}
				}
			}

			return 1; }));
	}

	for (auto&& result : results_dynamic) {
		result.get();
	}
	results_dynamic.clear();
}


void WeightDecreaseMaintenance(graph_hash_of_mixed_weighted& instance_graph, graph_hash_of_mixed_weighted_two_hop_case_info_v1& mm, int v1, int v2, weightTYPE w_new,
	ThreadPool& pool_dynamic, std::vector<std::future<int>>& results_dynamic) {

	std::vector<affected_label> CL_curr, CL_next;

	auto& L = mm.L;
	/*
	the following part does not suit parallel computation:
	the reason is that L is changed below, and as a result, in each following loop, L[v2] or L[v1] is locked at each step, 
	which means that following loops cannot be actually parallized

	//WeightDecreaseMaintenance_step1(v1, v2, w_new, &mm.L, &mm.PPR, &CL_curr, pool_dynamic, results_dynamic);
	*/
	for (int sl = 0; sl < 2; sl++) {
		if (sl == 1) {
			swap(v1, v2);
		}
		for (auto it : L[v1]) {
			int v = it.vertex;
			weightTYPE dis = it.distance + w_new;
			if (v <= v2) {
				auto query_result = Query2(v, v2); // query_result is {distance, common hub}
				if (query_result.first > dis) {
					insert_sorted_two_hop_label(L[v2], v, dis);
					CL_curr.push_back(affected_label(v2, v, dis));
				}
				else {
					auto search_result = search_sorted_two_hop_label2(L[v2], v);
					if (search_result.first != MAX_VALUE && search_result.first > dis) {
						L[v2][search_result.second].distance = dis;
						CL_curr.push_back(affected_label(v2, v, dis));
					}
					if (query_result.second != v) {
						PPR_insert(mm.PPR, v2, query_result.second, v);
					}
					if (query_result.second != v2) {
						PPR_insert(mm.PPR, v, query_result.second, v2);
					}
				}
			}
		}
	}


	while (CL_curr.size()) {
		ProDecreasep(&instance_graph, &mm.L, &mm.PPR, CL_curr, &CL_next, pool_dynamic, results_dynamic);
		CL_curr = CL_next;
		std::vector<affected_label>().swap(CL_next);	
	}
}


