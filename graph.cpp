#include "graph.hpp"

Graph::Graph() {
    num_v1 = num_v2 = m_ = t_ = max_deg_ = 0;
    nbr_v1_.clear();
    nbr_v2_.clear();
    sub_nbr_v1_.clear();
    sub_nbr_v2_.clear();
    bf_cnt_.clear();
    sub_bf_cnt_.clear();
    visit_.clear();
    two_hop_nbr_.clear();
    sub_two_hop_nbr_.clear();
    bfc_idx_.clear();
}

void Graph::read_graph(const string& path) {
    printf("Graph path: %s\n", path.c_str());
    ifstream ifs(path);
    if (!ifs.is_open()) {
        cerr << "Open file failed!" << endl;
        exit(-1);
    }

    int u, v;
    long ts, pre_ts = -1;
    while (!ifs.eof()) {
        ifs >> u >> v >> ts;
        if (ifs.fail()) break;  // 防止多读一个空行

        edges_.emplace_back(make_pair(u, v));

        // adjust size of neighbor list if necessary.
        if (u + 1 > nbr_t_[0].size()) {
            nbr_t_[0].resize(u + 1);
        }
        if (v + 1 > nbr_t_[1].size()) {
            nbr_t_[1].resize(v + 1);
        }

        if (ts != pre_ts) {
            // 一起添加的 两个下标一致
            t_new_to_old_.emplace_back(ts);
            edges_idx_.emplace_back(edges_.size() - 1);
            pre_ts = ts;
        }

        int format_t = t_new_to_old_.size() - 1;

        nbr_t_[0][u].emplace_back(make_pair(v, format_t));
        nbr_t_[1][v].emplace_back(make_pair(u, format_t));
        ++m_;
    }
    ifs.close();

    num_v1 = nbr_t_[0].size();
    num_v2 = nbr_t_[1].size();
    t_ = t_new_to_old_.size();
    edges_idx_.emplace_back(edges_.size());

    init(num_v1, num_v2);
    init_nbr_cnt();
}

void Graph::init(const ui num1, const ui num2) {
    bf_cnt_.resize(num1);
    nbr_v1_.resize(num1);
    nbr_v2_.resize(num2);
    two_hop_nbr_.resize(num1);
    bfc_idx_.resize(num1);

    for (int i = 0; i < num1; ++i) {
        bfc_idx_[i].resize(t_);
        for (int ts = 0; ts < t_; ++ts) {
            bfc_idx_[i][ts].resize(t_ - ts);
        }
    }
}

void Graph::init_nbr_cnt() {
    for (int u = 0; u < num_v1; ++u) {
        for (auto& i : nbr_t_[0][u]) {
            if (nbr_v1_[u].find(i.first) != nbr_v1_[u].end()) {
                ++nbr_v1_[u][i.first];
            } else {
                nbr_v1_[u].emplace(i.first, 1);
            }
        }
    }

    for (int u = 0; u < num_v2; ++u) {
        for (auto& i : nbr_t_[1][u]) {
            if (nbr_v2_[u].find(i.first) != nbr_v2_[u].end()) {
                ++nbr_v2_[u][i.first];
            } else {
                nbr_v2_[u].emplace(i.first, 1);
            }
        }
    }
}

void Graph::init_two_hop_common_neighbor() {
    for (int u = 0; u < num_v1; ++u) {
        for (auto& i : nbr_v1_[u]) {
            for (auto& j : nbr_v2_[i.first]) {
                if (j.first == u) continue;
                if (two_hop_nbr_[u].find(j.first) != two_hop_nbr_[u].end()) {
                    ++two_hop_nbr_[u][j.first];
                } else {
                    two_hop_nbr_[u].emplace(j.first, 1);
                }
            }
        }
    }
}

void Graph::index_bl() {
    init_two_hop_common_neighbor();

    for (int u = 0; u < num_v1; ++u) {
        for (auto& i : two_hop_nbr_[u]) {
            bf_cnt_[u] += (i.second * (i.second - 1)) / 2;
        }
    }

    for (int i = 0; i < num_v1; ++i) cout << bf_cnt_[i] << " ";
    cout << endl;

    for (int u = 0; u < num_v1; ++u) {
        bfc_idx_[u].resize(t_);
        for (int t = 0; t < t_; ++t) {
            bfc_idx_[u][t].resize(t_ - t);
        }
    }

    sub_bf_cnt_.resize(bf_cnt_.size());
    sub_nbr_v1_.resize(nbr_v1_.size());
    sub_nbr_v2_.resize(nbr_v2_.size());
    sub_two_hop_nbr_.resize(two_hop_nbr_.size());

    for (int t_s = 0; t_s < t_; ++t_s) {
        // if (t_s % 10 == 0) printf("t = %d.\n", t_s);
        copy(bf_cnt_.begin(), bf_cnt_.end(), sub_bf_cnt_.begin());
        copy(nbr_v1_.begin(), nbr_v1_.end(), sub_nbr_v1_.begin());
        copy(nbr_v2_.begin(), nbr_v2_.end(), sub_nbr_v2_.begin());
        copy(two_hop_nbr_.begin(), two_hop_nbr_.end(), sub_two_hop_nbr_.begin());

        compute_bfc_time_bl(t_s);

        if (t_s == t_ - 1) break;
        decremental_two_hop_cn_bl(t_s);
    }
}

void Graph::compute_bfc_time_bl(const int t_s) {
    for (int t_e = t_ - 1; t_e >= t_s; --t_e) {
        for (int e = edges_idx_[t_e]; e < edges_idx_[t_e + 1]; ++e) {
            int u = edges_[e].first;
            int v = edges_[e].second;

            --sub_nbr_v1_[u][v];
            --sub_nbr_v2_[v][u];

            if (sub_nbr_v1_[u][v] != 0) continue;
            
            sub_nbr_v1_[u].erase(v);
            sub_nbr_v2_[v].erase(u);

            for (auto& i : sub_nbr_v2_[v]) {
                int w = i.first;
                sub_bf_cnt_[u] -= --sub_two_hop_nbr_[u][w];
                sub_bf_cnt_[w] -= --sub_two_hop_nbr_[w][u];
            }
        }
        for (int u = 0; u < num_v1; ++u) bfc_idx_[u][t_s][t_e - t_s] = sub_bf_cnt_[u];
    }
}

void Graph::decremental_two_hop_cn_bl(const int t_s) {
    for (int i = edges_idx_[t_s]; i < edges_idx_[t_s + 1]; ++i) {
        int u = edges_[i].first;
        int v = edges_[i].second;

        --nbr_v1_[u][v];
        --nbr_v2_[v][u];

        if (nbr_v1_[u][v] != 0) continue;

        nbr_v1_[u].erase(v);
        nbr_v2_[v].erase(u);

        for (auto& i : nbr_v2_[v]) {
            int w = i.first;
            bf_cnt_[u] -= --two_hop_nbr_[u][w];
            bf_cnt_[w] -= --two_hop_nbr_[w][u];
        }
    }
}

void Graph::wirte_idx_txt() {
    ofstream fout("./result.txt", ios::out);

    fout.fill(' ');
    for (int u = 0; u < num_v1; ++u) {
        fout << "\n------------------- " << u << " -------------------" << endl;
        for (int t_s = 0; t_s < t_; ++t_s) {
            for (int i = 0; i < t_ - bfc_idx_[u][t_s].size(); ++i) fout << setw(4) << " ";
            for (int t_e = 0; t_e < bfc_idx_[u][t_s].size(); ++t_e) {
                fout << setw(4) << bfc_idx_[u][t_s][t_e];
            }
            fout << endl;
        }
    }
}

bool cmp1(const pair<int, int>& a, const pair<int, int>& b) {
    return a.first < b.first;
}
