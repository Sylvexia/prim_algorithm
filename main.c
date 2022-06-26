#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

// Prim's algorithm in c
// Adjacency list implemetation

// reference: https://www.geeksforgeeks.org/prims-mst-for-adjacency-list-representation-greedy-algo-6/

typedef struct minimum_spanning_tree_node
{
    int *parents;
    int *weights;

    int num_vertices;
    int total_weight;
} MSTree;

typedef struct adjacency_list_node
{
    int dest;
    int weight;
    struct adjacency_list_node *next;
} AdjListNode;

typedef struct adjacency_list
{
    AdjListNode *head;
} AdjList;

typedef struct graph
{
    int num_vertices;
    MSTree *mstree;
    AdjList *node_array;
} Graph;

typedef struct MinHeapNode
{
    int v;
    int weight;
} MinHeapNode;

typedef struct MinHeap
{
    int size;
    int capacity;
    int *index_pos;
    int *weights;
    MinHeapNode **node_array;
} MinHeap;

AdjListNode *newAdjListNode(int dest, int weight)
{
    AdjListNode *newNode = malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

void updateMSTree(MSTree *mstree, int parent, int child, int weight)
{
    mstree->parents[child] = parent;
    mstree->weights[child] = weight;
}

Graph *createGraph(int num_of_vertices)
{
    Graph *graph = malloc(sizeof(Graph));
    graph->num_vertices = num_of_vertices;

    graph->mstree = malloc(sizeof(MSTree));
    graph->mstree->parents = malloc(num_of_vertices * sizeof(int));
    graph->mstree->weights = malloc(num_of_vertices * sizeof(int));

    graph->node_array = malloc(num_of_vertices * sizeof(AdjList));

    for (int i = 0; i < num_of_vertices; i++)
    {
        graph->mstree->parents[i] = -1;
        graph->mstree->weights[i] = 0;
        graph->node_array[i].head = NULL;
    }

    graph->mstree->num_vertices = num_of_vertices;
    graph->mstree->total_weight = 0;

    return graph;
}

Graph *destroyGraph(Graph *graph)
{
    for (int i = 0; i < graph->num_vertices; i++)
    {
        AdjListNode *head = graph->node_array[i].head;
        while (head != NULL)
        {
            AdjListNode *temp = head;
            head = head->next;
            free(temp);
        }
    }
    free(graph->mstree->parents);
    free(graph->mstree->weights);
    free(graph->mstree);
    free(graph->node_array);
    free(graph);
    return NULL;
}

void addEdge(Graph *graph, int src, int dest, int weight)
{
    AdjListNode *newNode = newAdjListNode(dest, weight);

    newNode->next = graph->node_array[src].head;
    graph->node_array[src].head = newNode;

    newNode = newAdjListNode(src, weight);

    newNode->next = graph->node_array[dest].head;
    graph->node_array[dest].head = newNode;
}

MinHeapNode *newMinHeapNode(int v, int weight)
{
    MinHeapNode *minHeapNode = malloc(sizeof(MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->weight = weight;
    return minHeapNode;
}

MinHeap *createMinHeap(int capacity)
{
    MinHeap *minHeap = malloc(sizeof(MinHeap));
    minHeap->index_pos = malloc(capacity * sizeof(int));
    minHeap->weights = malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->node_array = malloc(capacity * sizeof(MinHeapNode *));
    return minHeap;
}

MinHeap *initMinHeap(MinHeap *minHeap)
{
    for (int v = 1; v < minHeap->capacity; v++)
    {
        minHeap->weights[v] = INT_MAX;
        minHeap->node_array[v] = newMinHeapNode(v, minHeap->weights[v]);
        minHeap->index_pos[v] = v;
    }

    minHeap->weights[0] = 0;
    minHeap->node_array[0] = newMinHeapNode(0, minHeap->weights[0]);
    minHeap->index_pos[0] = 0;

    minHeap->size = minHeap->capacity;
}

MinHeap *destroyMinHeap(MinHeap *minHeap)
{
    free(minHeap->node_array);
    free(minHeap->weights);
    free(minHeap->index_pos);
    free(minHeap);
    return NULL;
}

void swapMinHeap(MinHeap *minHeap, int a, int b)
{
    minHeap->index_pos[minHeap->node_array[a]->v] = b;
    minHeap->index_pos[minHeap->node_array[b]->v] = a;

    MinHeapNode *temp = minHeap->node_array[a];
    minHeap->node_array[a] = minHeap->node_array[b];
    minHeap->node_array[b] = temp;
}

void minHeapify(MinHeap *minHeap, int index)
{
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < minHeap->size && minHeap->node_array[left]->weight < minHeap->node_array[smallest]->weight) // if left child is smaller than parent
        smallest = left;

    if (right < minHeap->size && minHeap->node_array[right]->weight < minHeap->node_array[smallest]->weight) // if right child is smaller than parent
        smallest = right;

    if (smallest != index)
    {
        swapMinHeap(minHeap, smallest, index);
        minHeapify(minHeap, smallest);
    }
}

bool isEmpty(MinHeap *minHeap)
{
    return minHeap->size == 0;
}

MinHeapNode *extractMin(MinHeap *minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    swapMinHeap(minHeap, 0, minHeap->size - 1);
    --minHeap->size;

    minHeapify(minHeap, 0); // the root is the smallest element in the heap

    return minHeap->node_array[minHeap->size];
}

void updateHeap(MinHeap *minHeap, int v, int weight)
{
    int i = minHeap->index_pos[v];
    int parent = (i - 1) / 2;

    minHeap->node_array[i]->weight = weight;

    while (i && minHeap->node_array[i]->weight < minHeap->node_array[parent]->weight) // while i is not the root and the weight of the node is less than the weight of its parent
    {
        swapMinHeap(minHeap, i, parent);
        i = parent;
        parent = (i - 1) / 2;
    }
    // if the weight of the vertex v is the smallest weight in the heap, it would be swapped to the root
}

bool isInMinheap(MinHeap *minHeap, int v)
{
    if (minHeap->index_pos[v] < minHeap->size)
        return true;
    return false;
}

void printGraphAdjacencyList(Graph *graph)
{
    for (int i = 0; i < graph->num_vertices; i++)
    {
        AdjListNode *head = graph->node_array[i].head;
        printf("\n Adjacency list of vertex %d\n head ", i);
        while (head)
        {
            printf("-> %d", head->dest);
            head = head->next;
        }
        printf("\n");
    }
}

void printResult(Graph *graph)
{
    printf("Edge \tWeight\n");
    for (int i = 1; i < graph->num_vertices; i++)
    {
        printf("%d - %d \t%d\n", graph->mstree->parents[i], i, graph->mstree->weights[i]);
    }
    printf("total of the mst weight: %d\n", graph->mstree->total_weight);
}

void Prim_Algorithm(Graph *graph)
{
    MinHeap *minHeap = createMinHeap(graph->num_vertices);
    initMinHeap(minHeap);

    while (!isEmpty(minHeap))
    {
        MinHeapNode *minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // u is the vertex with the minimum weight
        AdjListNode *sweep = graph->node_array[u].head;

        while (sweep != NULL)
        {
            int current_v = sweep->dest; // v is the vertex that is connected to u
            int current_weight = sweep->weight;
            if (isInMinheap(minHeap, current_v) && current_weight < minHeap->weights[current_v]) // if the vertex v is in the min heap and the weight of the edge is less than the weight of the vertex v
            {
                updateMSTree(graph->mstree, u, current_v, current_weight); // update the current smallest weight of the vertex v
                minHeap->weights[current_v] = current_weight;              // update the weight of the vertex v
                updateHeap(minHeap, current_v, current_weight);            // update the min heap, the current smallest weight would be swapped to the root
            }
            sweep = sweep->next; // move to the next vertex
        }
    }

    for (int i = 1; i < graph->num_vertices; i++)
    {
        graph->mstree->total_weight += graph->mstree->weights[i];
    }

    destroyMinHeap(minHeap);

    return;
}

void generateRandConnectedGraph(Graph *graph, int num_vertices, int num_edges)
{
    if (num_edges < num_vertices - 1)
    {
        printf("The number of edges must be greater than the number of vertices - 1\n");
        exit(1);
    }

    int *rand_array = (int *)malloc(num_edges * sizeof(int));
    int count = 0;

    rand_array[0] = 0;

    for (int i = 1; i < num_vertices; i++)
    {
        rand_array[i] = i;
        int rand_index = rand() % i;

        int temp = rand_array[i];
        rand_array[i] = rand_array[rand_index];
        rand_array[rand_index] = temp;
    }

    for (int i = num_vertices; i < num_edges + 1; i++)
    {
        rand_array[i] = rand() % num_vertices;
        int rand_index = rand() % i;

        int temp = rand_array[rand_index];
        rand_array[rand_index] = rand_array[i];
        rand_array[i] = temp;
    }

    while (count < num_edges)
    {
        int u = rand_array[count];
        int v = rand_array[count + 1];
        int weight = rand() % 21 - 10;
        addEdge(graph, u, v, weight);
        count += 1;
    }

    free(rand_array);

    return;
}

void benchmark(int num_of_vertices, int num_of_edges)
{
    Graph *graph = createGraph(num_of_vertices);

    generateRandConnectedGraph(graph, num_of_vertices, num_of_edges);

    clock_t start = clock();
    Prim_Algorithm(graph);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("The time spent on Prim's algorithm is: %f\n", time_spent);
    printf("vertices: %d, edges: %d\n", num_of_vertices, num_of_edges);
    printf("Total weight of the mst is %d\n", graph->mstree->total_weight);

    destroyGraph(graph);
}

int main()
{
    srand(time(NULL));

    int num_of_vertices = 9;
    Graph *graph = createGraph(num_of_vertices);

    addEdge(graph, 0, 1, 4);
    addEdge(graph, 0, 7, 8);
    addEdge(graph, 1, 2, 8);
    addEdge(graph, 1, 7, 11);
    addEdge(graph, 2, 3, 7);
    addEdge(graph, 2, 8, 2);
    addEdge(graph, 2, 5, 4);
    addEdge(graph, 3, 4, 9);
    addEdge(graph, 3, 5, 14);
    addEdge(graph, 4, 5, 10);
    addEdge(graph, 5, 6, 2);
    addEdge(graph, 6, 7, 1);
    addEdge(graph, 6, 8, 6);
    addEdge(graph, 7, 8, 7);

    // printGraphAdjacencyList(graph);

    Prim_Algorithm(graph);

    printResult(graph);

    destroyGraph(graph);

    Graph *graph2 = createGraph(5);

    addEdge(graph2, 0, 1, 2);
    addEdge(graph2, 0, 3, 6);
    addEdge(graph2, 1, 2, 3);
    addEdge(graph2, 1, 3, 8);
    addEdge(graph2, 1, 4, 5);
    addEdge(graph2, 2, 4, 7);
    addEdge(graph2, 3, 4, 9);

    Prim_Algorithm(graph2);

    printResult(graph2);

    destroyGraph(graph2);

    benchmark(1000, 10000);

    benchmark(1000, 100000);
    benchmark(1000, 1000000);

    benchmark(10000, 1000000);
    benchmark(100000, 1000000);

    return 0;
}