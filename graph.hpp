#ifndef Graph_hpp
#define Graph_hpp

#include <string.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iomanip>

using namespace std;
#define ui unsigned int
// #define max(x, y) ((x) > (y) ? (x) : (y))
// #define min(x, y) ((x) < (y) ? (x) : (y))

class Graph {
    ui num_v1, num_v2, m_, t_, max_deg_;
    vector<long> t_new_to_old_;
    vector<int> edges_idx_;
    vector<pair<int, int>> edges_;
    vector<vector<pair<int, int>>> nbr_t_[2];

    vector<int> bf_cnt_;
    vector<int> sub_bf_cnt_;
    vector<bool> visit_;
    vector<unordered_map<int,int>> nbr_v1_;
    vector<unordered_map<int,int>> nbr_v2_;
    vector<unordered_map<int,int>> sub_nbr_v1_;
    vector<unordered_map<int,int>> sub_nbr_v2_;
    vector<unordered_map<int,int>> two_hop_nbr_;
    vector<unordered_map<int,int>> sub_two_hop_nbr_;

    vector<vector<vector<int>>> bfc_idx_;

    void init(const ui num1, const ui num2);
    void init_nbr_cnt();
    void init_two_hop_common_neighbor();
    void compute_bfc_time_bl(const int t_s);
    void decremental_two_hop_cn_bl(const int t_s);

   public:
    Graph();
    void read_graph(const string& path);
    void index_bl();
    void wirte_idx_txt();
};

bool cmp1(const pair<int,int> &a, const pair<int,int> &b);

#endif /* Graph_hpp */
