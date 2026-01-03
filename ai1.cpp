#include <iostream>
#include <stdio.h>
#include "board.hpp"
#include "cell_evaluate.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>

#define inf 1000000000
#define bonus 100
//tableの定義
enum Bound { EXACT, LOW,HIGH };

struct TTEntry {
    int depth;
    int value;
    Bound flag;  
};

unordered_map<board, TTEntry, board::hash> transpose_table;         
unordered_map<board, TTEntry, board::hash> former_transpose_table;
//訪問したノードをカウント
unsigned long long visited_nodes;

//moveordering用の評価値を算出
int moveordering_evaluate(const board& b) {
    auto it = former_transpose_table.find(b);
    if (it != former_transpose_table.end()) {
        return bonus - it->second.value;    
    }
    return evaluate(b);
}


// negaalpha法
int nega_alpha(board b, int depth, bool passed, int alpha, int beta) {
    visited_nodes++;
    auto it = transpose_table.find(b);
    if (it != transpose_table.end() )
        return it->second.value;

    if (depth == 0)
        return evaluate(b); 

    vector<board> child_nodes;
    for (int i = 0; i < hw2; ++i) {
        if (b.legal_place(i)) {
            board nb = b.move(i);
            nb.value = moveordering_evaluate(nb);
            child_nodes.push_back(nb);
        }
    }

    if (child_nodes.empty()) {
        if (passed) return evaluate(b);
        b.player = 1 - b.player;
        return -nega_alpha(b, depth, true, -beta, -alpha);
    }

    sort(child_nodes.begin(), child_nodes.end());

    int max_score = -inf;
    int g;
     bool first=true;
    for (const board& nb : child_nodes) {
        
     g = -nega_alpha(nb, depth - 1, false, -beta, -alpha);
        if (g >= beta){
         transpose_table[nb].value = g;
          transpose_table[nb].flag =EXACT;
            return g;
        }
        if(alpha<g)
        alpha = max(alpha, g);
        max_score = max(max_score, g);
 }
    

    transpose_table[b].value = max_score;//TTを更新（exact
    return max_score;
}
//negascout法
int nega_scout(board b, int depth, bool passed, int alpha, int beta) {
    visited_nodes++;
    auto it = transpose_table.find(b);
    if (it != transpose_table.end() && it->second.depth >=depth)//depthに有用性があるとき
    {
        int boader;
        if(it->second.flag == HIGH)
        {
         boader=it->second.value;
         alpha=max(alpha,boader);
        }
        else  if(it->second.flag == LOW){
          boader=it->second.value;
          beta=min(beta,boader);
        }
        else
         return it->second.value;
    }
    if (depth == 0)
        return evaluate(b);

    vector<board> child_nodes;
    for (int i = 0; i < hw2; ++i) {
        if (b.legal_place(i)) {
            board nb = b.move(i);
            nb.value = moveordering_evaluate(nb);
            child_nodes.push_back(nb);
        }
    }

    if (child_nodes.empty()) {
        if (passed) return evaluate(b);
        b.player = 1 - b.player;
        return -nega_scout(b, depth, true, -beta, -alpha);
    }

    sort(child_nodes.begin(), child_nodes.end());

    int max_score = -inf;
    int g;
    
     bool first=true;
    for (const board& nb : child_nodes) {
        auto it_n = transpose_table.find(nb);
        if(first){
         g = -nega_scout(nb, depth-1,false, -beta,-alpha);
         first=false;
    }else{
       g = -nega_scout(nb, depth - 1, false, -(alpha+1), -alpha);
          if (g > alpha && g < beta) {
            g = -nega_scout(nb, depth - 1, false, -beta, -alpha);
        }
    } 
 alpha = max(alpha, g);
        max_score = max(max_score, g);

if (g >= beta){//βかっと
        //TTに格納
         transpose_table[b].value = g;
         transpose_table[b].flag = HIGH;
         transpose_table[b].depth = depth-1;
            return g;
        }
    }
     if(alpha>max_score){
             //TTに格納
         transpose_table[b].value = beta;
         transpose_table[b].flag = LOW;
         transpose_table[b].depth = depth-1;
        }else {
             //TTに格納
         transpose_table[b].value = max_score;
         transpose_table[b].flag = EXACT;
         transpose_table[b].depth = depth-1;
        }
    
    return max_score;
}



// depth手読みの探索
int search(board b, int depth, int offset) {
    int start = std::max(1, depth - offset);
    int res = -1;

    for (int d = start; d <= depth; ++d) {
        int alpha = -inf, beta = inf;
        int best = -1;
          visited_nodes=0;
        for (int coord = 0; coord < hw2; ++coord) {
          
            if (!b.legal_place(coord)) continue;

            int score = -nega_scout(b.move(coord), d - 1, false, -beta, -alpha);

            if (score > alpha) {
                alpha = score;
                best = coord;
            }
        }

        res = best;

        // 今回の探索結果を次回の ordering 用に回す
        transpose_table.swap(former_transpose_table);
        transpose_table.clear();
    }

    return res;
}


