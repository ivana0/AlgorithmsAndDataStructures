/*
  CSCI203 Assignment 3 - Graphs - Shortest path
  Filename: assignment3.cpp
  By Ivana Ozakovic

*/

//REFERENCE:
//http://www.geeksforgeeks.org/greedy-algorithms-set-6-dijkstras-shortest-path-algorithm/

//http://www.geeksforgeeks.org/greedy-algorithms-set-7-dijkstras-algorithm-for-adjacency-list-representation/

//http://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/


//!!!SOLUTION
//b->m with distance 85 and the second shortest path: b->q->t->m with distance 134. 
//Is that right and is that all my program has to do?

// A C / C++ program for Dijkstra's single source shortest path algorithm.
// The program is for adjacency matrix representation of the graph
  
#include <iostream>
#include <fstream> 
#include <limits.h>


using namespace std;
  
//-------------------------    CONSTANTS      -------------------------------//
const int MAX_VERTICES = 1000;
const int MAX_EDGES = 1000;

//-------------------------    STRUCTS      -------------------------------//
//Vertex
struct Vertex{
  
  int id;
  char label;
  int x, y;
  
  char neighbours[MAX_VERTICES];    // IDs of neighbours.
  int numNeighbours;
  
  void addVertex(int idI, char l, int x1, int y1){
    id = idI;
    label = l;
    x = x1;
    y = y1;
    numNeighbours = 0;
  };

  //adds neighbour vertex
  bool addNeighbour(char id){

    neighbours[numNeighbours++] = id;
      return true;
  }

};

struct Edge{
  
  char vertex1, vertex2;
  int vertex1ID, vertex2ID;
  int weight;
  
  void addEdge(int v1ID, int v2ID, char v1, char v2, int w){
    vertex1ID = v1ID;
    vertex2ID = v2ID;
    vertex1 = v1;
    vertex2 = v2;
    weight = w;
  };
  
};

//-------------------------    GLOBALS      -------------------------------//

Vertex vertices[MAX_VERTICES];
Edge edges[MAX_EDGES];

//--------------------    FUNCTION DECLARATIONS      -----------------------//

void AStarAlgoritm();
void Dijkstra(int graph[][MAX_VERTICES], int source, int finish, const int numV);
int getVertexIndex(char id, const int numV);
void initGrid();
void createGrid(Vertex vertex);

//FROM REFERENCE:
int minDistance(int dist[], bool sptSet[], const int numV);
void printSolution(int dist[], const int numV, int parent[], int source, int finish);
void printPath(int parent[], int j);

// driver program to test above function
int main()
{

  ifstream ins;                               //input stream
  char filename[32];                          //variable for storing filename
    
  cout << "Enter an input filename: ";
  cin.getline(filename, sizeof filename);     //get filename
 
  ins.open(filename);                         //open file

  if(!ins.good())                             //if cannot open the file, print error message
  {
      cerr << "Unable to open the file. Program will now terminate." << endl;
      return 1;
  }

  

  int numVertices, numEdges;
  ins >> numVertices;
  ins >> numEdges;

  //GRAPH MATRIX
  int graph[MAX_VERTICES][MAX_VERTICES];

  char label;
  int x, y;
  
  for (int i = 0; i < numVertices; i++)
  {
    ins >> label;     //v1
    ins >> x;         //x coordinate
    ins >> y;         //y coordinate
    
    vertices[i].addVertex(i, label, x, y);
  }
  
  char v1, v2;
  int weight;
  for (int i = 0; i < numEdges; i++)
  {
    ins >> v1;        //v1
    ins >> v2;        //v2
    ins >> weight;    //wieght
    
    edges[i].addEdge(getVertexIndex(v1, numVertices), getVertexIndex(v2, numVertices), v1, v2, weight);
    vertices[getVertexIndex(v1, numVertices)].addNeighbour(v2);
    vertices[getVertexIndex(v2, numVertices)].addNeighbour(v1);
  }
  
  char start, goal;
  ins >> start;       //start
  ins >> goal;        //goal

  ins.close();


  //distance always 0 from the source
  
  //initialize all distances in matrix to 0 to indicate no edges
  for (int i = 0; i < numVertices; ++i)
  {
    for (int j = 0; i < numVertices; ++i)
    {
        graph[i][j] = 0;
    }
     
  }

  //insert edges and distances in the graph
  for (int x = 0; x < numEdges; ++x)
  {
      int i = edges[x].vertex1ID;
      int j = edges[x].vertex2ID;
      int w = edges[x].weight;

      graph[i][j] = w;
      graph[j][i] = w;
    
  }
  
  //int distance = 0;   //will be used store total distance

  

  //--------------------    PRINT STATS      -----------------------//
  cout << "Number of Edges: " << numEdges << endl;
  cout << "Number of Vertices: " << numVertices << endl;
  cout << endl;
  cout << "List of Vertices and their neighbours:" << endl;
    
  //PRINT VERTICES AND NEIGHBOURS 
  for (int i = 0; i < numVertices; i++)
  {
    
    cout << vertices[i].label << ": ";
    for (int j = 0; j < vertices[i].numNeighbours; j++){
      cout << vertices[i].neighbours[j] << " ";     
    }
    cout << endl;
  }
  cout << endl;
  cout << "List of Edges and their weight:" << endl;

  //PRINT VERTICES AND NEIGHBOURS 
  for (int i = 0; i < numEdges; i++)
  {
    cout << edges[i].vertex1 <<  " -> " << edges[i].vertex2 << "\t" << edges[i].weight << endl;
  }
  cout << endl;

  //--------------------    PRINT SOLUTION PATH STATS      -----------------------//
  cout << "//------------------   SOLUTION PATH STATS   ------------------//" << endl;
  cout << "Start: " << start << endl; 
  cout << "Goal: " << goal << endl; 
  
  cout << "Shortest Path solution" << endl;
  cout << "------------------------------------------------------------" << endl;
  Dijkstra(graph, getVertexIndex(start, numVertices), getVertexIndex(goal, numVertices), numVertices);
  
  cout << endl;
  
  return 0;
}


//--------------------    FUNCTION DEFINITIONS      -----------------------//
int getVertexIndex(char id, const int numV)
{
  for(int i = 0; i < numV; i++){
    if (vertices[i].label == id)
      return i; 
  }

  return -1;

}


//To find distance, find minDistanceNeighbour used for now
void AStarAlgoritm(){

  return;

}

void Dijkstra(int graph[][MAX_VERTICES], int source, int finish, const int numV)
{

    int dist[numV];  // The output array. dist[i] will hold
                  // the shortest distance from src to i
 
    // sptSet[i] will true if vertex i is included / in shortest
    // path tree or shortest distance from src to i is finalized
    bool sptSet[numV];
 
    // Parent array to store shortest path tree
    int parent[numV];
 
    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < numV; i++)
    {
        parent[0] = -1;
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
 
    // Distance of source vertex from itself is always 0
    dist[0] = 0;
 
    // Find shortest path for all vertices
    for (int count = 0; count < (numV-1); count++)
    {
        // Pick the minimum distance vertex from the set of
        // vertices not yet processed. u is always equal to src
        // in first iteration.
        int u = minDistance(dist, sptSet, numV);
 
        // Mark the picked vertex as processed
        sptSet[u] = true;
 
        // Update dist value of the adjacent vertices of the
        // picked vertex.
        for (int v = 0; v < numV; v++)
 
            // Update dist[v] only if is not in sptSet, there is
            // an edge from u to v, and total weight of path from
            // src to v through u is smaller than current value of
            // dist[v]
            if (!sptSet[v] && graph[u][v] &&
                dist[u] + graph[u][v] < dist[v])
            {
                parent[v]  = u;
                dist[v] = dist[u] + graph[u][v];
            }  
    }
 

    // print the constructed distance array
    printSolution(dist, numV, parent, source, finish);


}


//--------------------    REFERENCED FUNCTION DEFINITIONS      -----------------------//


// A utility function to find the vertex with minimum distance
// value, from the set of vertices not yet included in shortest
// path tree
int minDistance(int dist[], bool sptSet[], const int numV)
{
    // Initialize min value
    int min = INT_MAX, min_index;
 
    for (int v = 0; v < numV; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
 
    return min_index;
}
 
// Function to print shortest path from source to j
// using parent array
void printPath(int parent[], int j)
{
    // Base Case : If j is source
    if (parent[j] == -1)
        return;
 
    printPath(parent, parent[j]);
 
    printf("%c ", vertices[j].label);
}
 
// A utility function to print the constructed distance
// array
void printSolution(int dist[], const int numV, int parent[], int source, int finish)
{
    //int src = 0;
    printf("From -> To\t\tDistance\tPath");
    cout << endl;
    cout << "------------------------------------------------------------";
    for (int i = 1; i < numV; i++)
    {
        if(vertices[i].label == vertices[finish].label)
        {
          printf("\n%c -> %c \t\t\t %d\t\t%c ", vertices[source].label, vertices[i].label, dist[i], vertices[source].label);
          printPath(parent, i);
        }
    }
}

