// Programming Assignment #1: Uninformed and Informed Search
// By: Juichi Lee, Michael Boly
// Spring 2022
// CS 331 Intro to Artificial Intelligence
// 4/18/2022

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <array>
#include <queue>
#include <limits.h>

using namespace std;

struct node {
  vector<int> leftBank; //(#chickens, # wolves, #boats)
  vector<int> rightBank;
  node* previous;
  int fVal;
};

// Prototypes
int bfsGraphSearch(node*, node*, vector<node*>&);
int dfsGraphSearch(node*, node*, vector<node*>&);
int iddfsGraphSearch(node*, node*, vector<node*>&);
int aStarGraphSearch(node*, node*, vector<node*>&);

void print(vector<int> const&);
void printNode(node, ofstream&);
void printSolution(char*, int, vector<node*>, char*);
void readStringIntoArray(string, vector<int>&);
bool isValidState(vector<int>);
void checkAndAddToFrontier(vector<node*>, vector<node*>&, vector<node*>);
void processActions(vector<node*> &, node*);
bool checkNodeMatch(node*, vector<node*>);

fstream inStart, inGoal;

//Functions
void print(std::vector<int> const &a) {
 std::cout << "The vector elements are : ";

  for(int i=0; i < a.size(); i++)
    std::cout << a.at(i) << ' ';

  cout << endl;
}

void readStringIntoArray(string readString, vector<int>& array){
  char* token = strtok(&readString[0], ",");
  while(token != NULL){
    array.push_back(atoi(token));
    token = strtok(NULL, ",");
  }
}

int main(int argc, char* argv[]) {

  if(argc != 5){
    cout << "Not enough arguments!" << endl;
  }else{
    inStart.open(argv[1]);
    inGoal.open(argv[2]);
    
    node* initialState = new node();
    node* goalState = new node();
    
    if(inStart.is_open() && inGoal.is_open()){
      string leftBankStr;
      string rightBankStr;
      getline(inStart, leftBankStr);
      getline(inStart, rightBankStr);
      readStringIntoArray(leftBankStr, initialState->leftBank);
      readStringIntoArray(rightBankStr, initialState->rightBank);

      getline(inGoal, leftBankStr);
      getline(inGoal, rightBankStr);
      readStringIntoArray(leftBankStr, goalState->leftBank);
      readStringIntoArray(rightBankStr, goalState->rightBank);
      
      vector<node*> solution;
  
      string searchType(argv[3]);
      int numNodesExpanded;
      
      if(searchType.compare("bfs") == 0){
        cout << "BFS" << endl;
        numNodesExpanded = bfsGraphSearch(initialState, goalState, solution);
        
      }
      if(searchType.compare("dfs") == 0){
        cout << "DFS" << endl;
        numNodesExpanded = dfsGraphSearch(initialState, goalState, solution);
        
      }
      if(searchType.compare("iddfs") == 0){
        cout << "IDDFS" << endl;
        numNodesExpanded = iddfsGraphSearch(initialState, goalState, solution);
        
      }
      if(searchType.compare("astar") == 0){
        cout << "ASTAR" << endl;
        numNodesExpanded = aStarGraphSearch(initialState, goalState, solution);
      }
  
      printSolution(argv[3], numNodesExpanded, solution, argv[4]);
      inStart.close();
      inGoal.close();
    }
  }
  
  return 0;
}

void printSolution(char* mode, int numNodesExpanded, vector<node*> solution, char* outName){
  ofstream outFile;
  outFile.open(outName);
  cout << "Mode: " << mode << endl;
  outFile << "Mode: " << mode << endl;
  
  cout << "Number of Nodes Expanded: " << numNodesExpanded << endl;
  outFile << "Number of Nodes Expanded: " << numNodesExpanded << endl;
  
  int solutionSize = solution.size();
  cout << "Solution Path(chronological order): " << endl;
  outFile << "Solution Path(chronological order): " << endl;
  
  for(int i = 0; i < solutionSize; i++){
    cout << i << ": " << endl;
    outFile << i << ": " << endl;
    
    printNode(*solution.back(), outFile);
    solution.pop_back();
    cout << endl;
    outFile << endl;
  }

  cout << "Number of Nodes Expanded: " << numNodesExpanded << endl;
  outFile << "Number of Nodes Expanded: " << numNodesExpanded << endl;
  
  outFile.close();
}

void printNode(node node, ofstream& outFile){
  cout << "Left: ";
  outFile << "Left: ";
  
  for(int i = 0; i < node.leftBank.size(); i++){
    cout << node.leftBank[i] << ", ";
    outFile << node.leftBank[i] << ", ";
  }
  
  cout << "\nRight: ";
  outFile << "\nRight: ";
  
  for(int i = 0; i < node.leftBank.size(); i++){
    cout << node.rightBank[i] << ", ";
    outFile << node.rightBank[i] << ", ";
  }
  cout << "\n";
  outFile << "\n";
}

int bfsGraphSearch(node* startNode, node* goalNode, vector<node*>& solution) {

  int numNodesExpanded = 0;
  // returns -1 if failure, numNodesExpanded if solution found
  //initialize frontier using initial state FIFO Queue
  vector<node*> frontier;
  frontier.push_back(startNode);
  // initialize the explored set to be empty
  vector<node*> exploredSet;
  vector<node*> successorNodes;

  //setup solution stuff

  while (true) {
    int frontierLength = frontier.size();
    if (frontierLength == 0) { //fail if empty
      return -1;
    }

    //choose a leaf node and remove it from the frontier
    //in BFS, we choose the first one in the array 
    // Using the vector, the direction is (earliest) front -> back (latest)
    node* leafNode = frontier.front();
    frontier.erase(frontier.begin());
    numNodesExpanded++;
    
    //if node contains a goal state then return the solution
    if (leafNode->leftBank == goalNode->leftBank &&
        leafNode->rightBank == goalNode->rightBank) {
      cout << "FOUND SOLUTION" << endl;
      //build solution here using predecessor nodes
      solution.push_back(leafNode);
      node* parent = leafNode->previous;
      while (parent != NULL) {

        //add successor to the solution then move on to next one
        solution.push_back(parent);
        parent = parent->previous;
      }
      return numNodesExpanded-1; 
    }

    //add node to explored set
    exploredSet.push_back(leafNode);

    //Queue to hold all the successor nodes
    successorNodes.clear();

    //expand the chosen node
    processActions(successorNodes, leafNode);
    
    //add resulting nodes to the frontier if not in frontier or explored set
    checkAndAddToFrontier(successorNodes, frontier, exploredSet);

  }
  return -1;
}

int dfsGraphSearch(node* startNode, node* goalNode, vector<node*>& solution) {
  int numNodesExpanded = 0;
  // returns -1 if failure, numNodesExpanded if solution found
  //initialize frontier using initial state LIFO Queue
  vector<node*> frontier;
  frontier.push_back(startNode);
  // initialize the explored set to be empty
  vector<node*> exploredSet;
  vector<node*> successorNodes;

  vector<int> goalState = startNode->rightBank;
  //setup solution stuff
  
  while (true) {
    int frontierLength = frontier.size();
    if (frontierLength == 0) { //fail if empty
      return -1;
    }
  
    //choose a leaf node and remove it from the frontier
    //in DFS, we choose the last one in the array 
    // Using the vector, the direction is (earliest) front -> back (latest)
    node* leafNode = frontier.back();
    frontier.pop_back();
    numNodesExpanded++;
    
    //if node contains a goal state then return the solution
    if (leafNode->leftBank == goalNode->leftBank &&
        leafNode->rightBank == goalNode->rightBank) {
      cout << "FOUND SOLUTION" << endl;
      //build solution here using predecessor nodes
      solution.push_back(leafNode);
      node* parent = leafNode->previous;
      while (parent != NULL) {
  
        //add successor to the solution then move on to next one
        solution.push_back(parent);
        parent = parent->previous;
      }
      return numNodesExpanded-1;
    }
  
    //add node to explored set
    exploredSet.push_back(leafNode);
  
    //Queue to hold all the successor nodes
    successorNodes.clear();
    
    //expand the chosen node
    processActions(successorNodes, leafNode);
  
    //add resulting nodes to the frontier if not in frontier or explored set
    checkAndAddToFrontier(successorNodes, frontier, exploredSet);

  }
  return -1;
}

int iddfsGraphSearch(node* startNode, node* goalNode, vector<node*>& solution) {
  int numNodesExpanded = 0;
  // returns -1 if failure, numNodesExpanded if solution found
  int depthLimit;
  // run DFS 0,1,2..until goal is found
  for (depthLimit = 0; depthLimit < INT_MAX; depthLimit++) {
    //depth counter
    int currentDepth = 0;
  
    //initialize frontier using initial state LIFO Queue
    vector<node*> frontier;
    frontier.push_back(startNode);
    // initialize the explored set to be empty
    vector<node*> exploredSet;
    vector<node*> successorNodes;
  
    vector<int> goalState = startNode->rightBank;
    //setup solution stuff
    
    while (currentDepth <= depthLimit) {
      //cout << "currentDepth: " << currentDepth << endl;
      
      int frontierLength = frontier.size();
      if (frontierLength == 0) { //fail if empty
        return -1;
      }
    
      //choose a leaf node and remove it from the frontier
      //in DFS, we choose the last one in the array 
      // Using the vector, the direction is (earliest) front -> back (latest)
      node* leafNode = frontier.back();
      frontier.pop_back();
      numNodesExpanded++;
      
      //if node contains a goal state then return the solution
      if (leafNode->leftBank == goalNode->leftBank &&
        leafNode->rightBank == goalNode->rightBank) {
        cout << "FOUND SOLUTION" << endl;
        //build solution here using predecessor nodes
        solution.push_back(leafNode);
        node* parent = leafNode->previous;
        while (parent != NULL) {
    
          //add successor to the solution then move on to next one
          solution.push_back(parent);
          parent = parent->previous;
        }
        return numNodesExpanded-1;
      } 
  
      //increment depth
      currentDepth++;
    
      //add node to explored set
      exploredSet.push_back(leafNode);
    
      //Queue to hold all the successor nodes
      successorNodes.clear();
      
      //expand the chosen node
      processActions(successorNodes, leafNode);
    
      //add resulting nodes to the frontier if not in frontier or explored set
      checkAndAddToFrontier(successorNodes, frontier, exploredSet);

    }
  }
  return -1;
}

int aStarGraphSearch(node* startNode, node* goalNode, vector<node*>& solution) {

  int numNodesExpanded = 0;
  // returns -1 if failure, numNodesExpanded if solution found
  //initialize frontier using initial state Priority Queue
  vector<node*> frontier;
  frontier.push_back(startNode);
  // initialize the explored set to be empty
  vector<node*> exploredSet;
  vector<node*> successorNodes;

  vector<int> goalState = startNode->rightBank;
  //setup solution stuff

  //depth counter
    int currentDepth = 0;
  
  while (true) {
    int frontierLength = frontier.size();
    if (frontierLength == 0) { //fail if empty
      return -1;
    }
  
    //choose a leaf node and remove it from the frontier
    //in A* search, we choose the node with the minimum f value (f = g + h) 
    //g = distance from startNode to that node
    //h = distance from that node to goalState

    //calculate f value for each node in frontier
    for(node* i : frontier) {
      int g = currentDepth; // *not sure if current Depth is an accurate count of the number of steps to get to the current node from the beginning. 
      int h = i->rightBank[0] + i->rightBank[1];
      int f = g + h; 

      i->fVal = f; 
    }

    //create priority queue and push frontier
    priority_queue <node*, vector<node*>, greater<node*>> priorityFrontier;
    for(node* i : frontier) {
      priorityFrontier.push(i);
    }
    
    // Using the priority queue, chose node with smallest f value
    node* leafNode = priorityFrontier.top();
    // pop the same node from frontier as we just got from the priority queue
    int index = 0;
    for(node* i : frontier) {
      if(i->leftBank == leafNode->leftBank &&
      i->rightBank == leafNode->rightBank){
        frontier.erase (frontier.begin() + index);
      }
      index++;
    }
    numNodesExpanded++;

    // The above is somewhat ugly (making a piority queue, pushing the frontier, and recalculating the fVal every time), but I did it to avoid having to change all the functions that use a vector frontier to take a priorityQueue parameter instead
    
    //if node contains a goal state then return the solution
    if (leafNode->leftBank == goalNode->leftBank &&
        leafNode->rightBank == goalNode->rightBank) {
      cout << "FOUND SOLUTION" << endl;
      //build solution here using predecessor nodes
      solution.push_back(leafNode);
      node* parent = leafNode->previous;
      while (parent != NULL) {
  
        //add successor to the solution then move on to next one
        solution.push_back(parent);
        parent = parent->previous;
      }
      return numNodesExpanded-1;
    }

    //increment depth
    currentDepth++;
  
    //add node to explored set
    exploredSet.push_back(leafNode);
  
    //Queue to hold all the successor nodes
    successorNodes.clear();
    
    //expand the chosen node
    processActions(successorNodes, leafNode);
  
    //add resulting nodes to the frontier if not in frontier or explored set
    checkAndAddToFrontier(successorNodes, frontier, exploredSet);

  }
  return -1;
}

void processActions(vector<node*>& successorNodes, node* parentNode){  // Takes in info from either bank and returns 
  //a list of successor nodes
  vector<int> leftBank = parentNode->leftBank;
  vector<int> rightBank = parentNode->rightBank;

  vector<int> thisBankc;
  vector<int> otherBankc;

  for(int i = 0; i < 5; i++){ // One loop for testing each case
    if(leftBank[2] == 1){ // If the leftBank has the boat, set thisBankc to leftBank
      thisBankc = leftBank;
      otherBankc = rightBank;
    }else{
      thisBankc = rightBank;
      otherBankc = leftBank;
    }

    //cout << "Ending Banks" << endl;
    node* newNode = new node(); 
    switch(i){
      case 0: // Put one chicken in the boat
        thisBankc[0] = thisBankc[0]-1; 
        otherBankc[0] = otherBankc[0]+1;
        thisBankc[2] = 0; 
        otherBankc[2] = 1;
        break;
      case 1: // Put two chickens in the boat 
        thisBankc[0] = thisBankc[0]-2; 
        otherBankc[0] = otherBankc[0]+2;
        thisBankc[2] = 0; 
        otherBankc[2] = 1;
        break; 
      case 2: // Put one wolf in the boat
        thisBankc[1] = thisBankc[1]-1; 
        otherBankc[1] = otherBankc[1]+1;
        thisBankc[2] = 0; 
        otherBankc[2] = 1;
        break;
      case 3: // Put one chicken and one wolf in the boat
        thisBankc[0] = thisBankc[0]-1; 
        otherBankc[0] = otherBankc[0]+1;
        thisBankc[1] = thisBankc[1]-1; 
        otherBankc[1] = otherBankc[1]+1;
        thisBankc[2] = 0; 
        otherBankc[2] = 1;
        break;
      case 4: // Put two wolves in the boat
        thisBankc[1] = thisBankc[1]-2; 
        otherBankc[1] = otherBankc[1]+2;
        thisBankc[2] = 0; 
        otherBankc[2] = 1;
        break;
    }

    if(isValidState(thisBankc) && isValidState(otherBankc)){ // Check if this bank is still valid. If so, add to successor nodes
      if(leftBank[2] == 1){ // Match and set the temp variables to newNode's leftBank and rightBank
        newNode->leftBank = thisBankc;
        newNode->rightBank = otherBankc;
      }else{
        newNode->leftBank = otherBankc;
        newNode->rightBank = thisBankc;
      }
      newNode->previous = parentNode;
      successorNodes.push_back(newNode);
    }
  }
}

bool isValidState(vector<int> testArray){ // Takes in a vector and checks if the vector is in a valid configuration
  int numChickens = testArray[0];
  int numWolves = testArray[1];

  if(numChickens >= 0 && numWolves >= 0){ // Check if numChickens and numWolves are greater than equal to zero
    if(numChickens == 0 || numChickens != 0 && numChickens >= numWolves){ // If num chickens is equal to zero or is greater than zero and is greater than equal to num wolves, configuration is valid
      return true;
    }else{
      return false; // Otherwise false
    }
  }else{
    return false;
  }
}

void checkAndAddToFrontier(vector<node*> successorNodes, vector<node*>& frontier, vector<node*> exploredSet){
  int successorSize = successorNodes.size();
  int exploredSize = exploredSet.size();
  int frontierSize = frontier.size();
  
  for(int i = 0; i < successorSize; i++){
    node* currNode = successorNodes[i];

    // First checking if currNode is in the explored set
    bool alreadyExplored = checkNodeMatch(currNode, exploredSet);
    
    // If the currNode is already in the explored set, continue to next node
    if(alreadyExplored){
      continue;
    }
    
    // Checking if the currNode is in the frontier
    bool inFrontier = checkNodeMatch(currNode, frontier);

    // If the currNode is already in the frontier, continue to next node
    if(inFrontier){
      continue; 
    } 
    // If currNode is not in explored set nor the frontier, then add to frontier
    frontier.push_back(currNode);
  }
}

bool checkNodeMatch(node* currNode, vector<node*> nodeList){
  int size = nodeList.size();
  node* otherNode;
  for(int j = 0; j < size; j++){
    node* otherNode = nodeList[j];
    if(currNode->leftBank == otherNode->leftBank &&
      currNode->rightBank == otherNode->rightBank){
      return true;
    } 
  }
  return false;
}
