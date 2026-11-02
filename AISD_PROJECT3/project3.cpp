#include <cstdio>
#define INF 1000000000000000000LL

struct Edge {
    int to;
    long long weight;
    Edge* next;
};

struct Graph{
    int n;
    int m;
    int count;
    Edge** head;
    Edge* edges;
};

struct Matching{
    int* matchRight;
    int* matchLeft;
    long long* rowReduce;
    long long* colReduce;
    bool* visitedLeft;
    bool* visitedRight;
    long long* deficit;
    int* deficitFrom;
};

void addEdge(Graph& graph, int u, int v, long long w){
    Edge* edge = &graph.edges[graph.count++];
    edge->to = v;
    edge->weight = w;   
    edge->next = graph.head[u];
    graph.head[u] = edge;
}

void colorGraph(Graph& graph, int* color){
    int n = graph.n;
    for (int i = 0; i <= n; i++){
        color[i] = -1;
    }
    int* queue = new int[n+1];
    int qHead = 0;
    int qTail = 0;
    for (int u = 1; u <= n; u++){
        if (color[u] == -1){
            color[u] = 0;
            queue[qTail++] = u;
            while (qHead < qTail){
                int x = queue[qHead++];
                for (Edge* edge = graph.head[x]; edge; edge = edge->next){
                    int v = edge->to;
                    if (color[v] == -1){
                        color[v] = color[x] ^ 1;
                        queue[qTail++] = v;
                    }
                }
            }
        }
    }
    delete[] queue;
}

void matchingSetup(Matching& match, int n){
    match.matchRight = new int[n+1];
    match.matchLeft = new int[n+1];
    match.rowReduce = new long long[n+1];
    match.colReduce = new long long[n+1];
    match.visitedLeft = new bool[n+1];
    match.visitedRight = new bool[n+1];
    match.deficit = new long long[n+1];
    match.deficitFrom = new int[n+1];
}

int countSides(int* color, int n){
    int cntL = 0, cntR = 0;
    for (int i = 1; i <= n; i++){
        if (color[i] == 0){
            cntL++;
        }
        else if (color[i] == 1){
            cntR++;
        }
    }
    if (cntL <= cntR){
        return 0;
    }
    else {
        return 1;
    }
}
void resetPhase(Matching& match, int n){
    for (int i = 0; i <= n; i++){
        match.visitedLeft[i] = false;
        match.visitedRight[i] = false;
        match.deficit[i] = INF;
        match.deficitFrom[i] = -1;
    }
}
int findMinDeficit(Matching& match, int n){
    int best = -1;
    for (int v = 0; v <= n; v++){
        if (match.visitedRight[v]){
            continue;
        }
        if (best == -1 || match.deficit[v] < match.deficit[best]){
            best = v;
        }
    }
    return best;
}
void updateLabels(Matching& match, int n, long long delta){
    for (int i = 0; i <= n; i++){
        if (match.visitedLeft[i]){
            match.rowReduce[i] += delta;
        }
        if (match.visitedRight[i]){
            match.colReduce[i] -= delta;
        }
        else if (match.deficit[i] < INF){
            match.deficit[i] -= delta;
        }
    }
}
void updateDeficit(Matching& match, Graph& graph, int u){
    for (Edge* edge = graph.head[u]; edge; edge = edge->next){
        int v = edge->to;
        if (match.visitedRight[v]){
            continue;
        }
        long long reduced = edge->weight - match.rowReduce[u] - match.colReduce[v];
        if (reduced < match.deficit[v]){
            match.deficit[v] = reduced;
            match.deficitFrom[v] = u;
        }
    }
    if (!match.visitedRight[0]){
        long long reduced0 = 0 - match.rowReduce[u] - match.colReduce[0];
        if (reduced0 < match.deficit[0]){
            match.deficit[0] = reduced0;
            match.deficitFrom[0] = u;
        }
    }
}
void rematch(Matching& match, int v){
    while (v != -1){
        int u = match.deficitFrom[v];
        int vNext = match.matchLeft[u];
        match.matchRight[v] = u;
        match.matchLeft[u] = v;
        v = vNext;
    }
}
void findPair(Matching& match, Graph& graph, int start, int n){
    resetPhase(match, n);
    match.visitedLeft[start] = true;
    updateDeficit(match, graph, start);
    while(true){
        int v = findMinDeficit(match, n);
        if (v == -1){
            break;
        }
        long long delta = match.deficit[v];
        if (delta >= INF){
            break;
        }
        updateLabels(match, n, delta);
        match.visitedRight[v] = true;
        if (v == 0) {
            rematch(match, v);
            break;
        }
        int u = match.matchRight[v];
        if (u == -1){
            rematch(match, v);
            break;
        }
        match.visitedLeft[u] = true;
        updateDeficit(match, graph, u);
    }
}
long long edgeWeight(Graph& graph, int u, int v){
    for (Edge* edge = graph.head[u]; edge; edge = edge->next){
        if (edge->to == v){
            return -edge->weight;
        }
    }
    return 0;
}

long long maxPotential(Matching& match, Graph& graph, int* color, int n){
    for (int i = 0; i <= n; i++){
        match.matchRight[i] = -1;
        match.matchLeft[i] = -1;
        match.rowReduce[i] = 0;
        match.colReduce[i] = 0;
        long long min_w = 0;
        for (Edge* edge = graph.head[i]; edge; edge = edge->next){  
            if (edge->weight < min_w) {
                min_w = edge->weight;
            }
        }
        match.rowReduce[i] = min_w;
    }
    int iterColor = countSides(color, n);
    for (int u = 1; u <= n; u++){
        if (color[u] == iterColor){
            findPair(match, graph, u, n);
        }
    }
    long long result = 0;
    for (int u = 1; u <= n; u++){
        if (color[u] == iterColor && match.matchLeft[u] > 0){
            result += edgeWeight(graph, u, match.matchLeft[u]);
        }
    }
    return result;
}

void matchingFree(Matching& match){
    delete[] match.matchRight;
    delete[] match.matchLeft;
    delete[] match.rowReduce;
    delete[] match.colReduce;
    delete[] match.visitedLeft;
    delete[] match.visitedRight;
    delete[] match.deficit;
    delete[] match.deficitFrom;
}

int main(){

    int t;
    scanf("%d", &t);
    while(t--){
        int n, m;
        scanf("%d %d", &n, &m);
        Graph graph;
        graph.n = n;
        graph.m = m;
        graph.head = new Edge*[n+1];
        for (int i = 0; i <=n; i++){
            graph.head[i] = nullptr;
        }
        graph.edges = new Edge[2*m];
        graph.count = 0;
        int* color = new int[n+1];
        for (int i = 0; i < m; i++){
            int u, v;
            long long w;
            scanf("%d %d %lld", &u, &v, &w);
            addEdge(graph, u, v, -w);
            addEdge(graph, v, u, -w);
        }
        colorGraph(graph, color);

        Matching match;
        matchingSetup(match, n);

        long long result = maxPotential(match, graph, color, n);
        printf("%lld\n", result);

        delete[] graph.head;
        delete[] graph.edges;
        delete[] color;
        matchingFree(match);
    }

    return 0;
}