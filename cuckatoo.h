// Header guard
#ifndef CUCKATOO_H
#define CUCKATOO_H


// Header files
using namespace std;


// Structures

// Cuckatoo node connections link structure
struct CuckatooNodeConnectionsLink {

	// Previous node connection link
	const CuckatooNodeConnectionsLink *previousNodeConnectionLink;
	
	// Node
	uint32_t node;
	
	// Edge index
	uint32_t edgeIndex;
};


// Global variables

// Cuckatoo newest node connections
thread_local static HashTable<CuckatooNodeConnectionsLink, MAX_NUMBER_OF_EDGES_AFTER_TRIMMING> cuckatooUNewestNodeConnections;
thread_local static HashTable<CuckatooNodeConnectionsLink, MAX_NUMBER_OF_EDGES_AFTER_TRIMMING> cuckatooVNewestNodeConnections;

// Cuckatoo visited pairs
thread_local static HashTable<uint32_t, SOLUTION_SIZE / 2> cuckatooUVisitedNodePairs;
thread_local static HashTable<uint32_t, SOLUTION_SIZE / 2> cuckatooVVisitedNodePairs;

// Cuckatoo root node
thread_local static uint32_t cuckatooRootNode;


// Function prototypes

// Initialize cuckatoo thread local global variables
static inline bool initializeCuckatooThreadLocalGlobalVariables() noexcept;

// Get cuckatoo solution
static inline bool getCuckatooSolution(uint32_t solution[SOLUTION_SIZE], CuckatooNodeConnectionsLink *nodeConnections, const uint32_t *edges, const uint64_t numberOfEdges) noexcept;

// Search node connections for cuckatoo solution first partition
static inline bool searchNodeConnectionsForCuckatooSolutionFirstPartition(const uint_fast8_t cycleSize, const uint32_t node, const uint32_t *index) noexcept;

// Search node connections for cuckatoo solution second partition
static inline bool searchNodeConnectionsForCuckatooSolutionSecondPartition(const uint_fast8_t cycleSize, const uint32_t node, const uint32_t *index) noexcept;


// Supporting function implementation

// Initialize cuckatoo thread local global variables
bool initializeCuckatooThreadLocalGlobalVariables() noexcept {

	// Check if creating thread local global variables failed
	if(!cuckatooUNewestNodeConnections || !cuckatooVNewestNodeConnections || !cuckatooUVisitedNodePairs || !cuckatooVVisitedNodePairs) {
	
		// Return false
		return false;
	}
	
	// Reset thread local global variables
	cuckatooUNewestNodeConnections.clear();
	cuckatooVNewestNodeConnections.clear();
	cuckatooUVisitedNodePairs.clear();
	cuckatooVVisitedNodePairs.clear();
	cuckatooRootNode = 0;
	
	// Retrn true
	return true;
}

// Get cuckatoo solution
bool getCuckatooSolution(uint32_t solution[SOLUTION_SIZE], CuckatooNodeConnectionsLink *nodeConnections, const uint32_t *edges, const uint64_t numberOfEdges) noexcept {

	// Go through all edges
	for(uint64_t nodeConnectionsIndex = 0, edgesIndex = 0; nodeConnectionsIndex < numberOfEdges * 2; nodeConnectionsIndex += 2, edgesIndex += EDGE_NUMBER_OF_COMPONENTS) {
		
		// Get edge's index and nodes
		const uint32_t *index = &edges[edgesIndex];
		uint32_t node = edges[edgesIndex + 1];
		cuckatooRootNode = edges[edgesIndex + 2];
		
		// Replace newest node connection for the node on the first partition and add node connection to list
		nodeConnections[nodeConnectionsIndex] = {cuckatooUNewestNodeConnections.replace(node, &nodeConnections[nodeConnectionsIndex]), node, edges[edgesIndex]};
		
		// Replace newest node connection for the node on the second partition and add node connection to list
		nodeConnections[nodeConnectionsIndex + 1] = {cuckatooVNewestNodeConnections.replace(cuckatooRootNode, &nodeConnections[nodeConnectionsIndex + 1]), cuckatooRootNode, edges[edgesIndex]};
		
		// Check if both nodes have a pair
		if(cuckatooUNewestNodeConnections.contains(node ^ 1) && cuckatooVNewestNodeConnections.contains(cuckatooRootNode ^ 1)) {
		
			// Reset visited nodes
			cuckatooUVisitedNodePairs.clear();
			cuckatooVVisitedNodePairs.clear();
			
			// Go through all nodes in the cycle
			for(uint_fast8_t cycleSize = 1;; cycleSize += 2) {
				
				// Set that node pair has been visited
				cuckatooUVisitedNodePairs.setUnique(node >> 1, index);
				
				// Check if node's pair has more than one connection
				const CuckatooNodeConnectionsLink *nodeConnection = cuckatooUNewestNodeConnections.get(node ^ 1);
				if(nodeConnection->previousNodeConnectionLink) {
				
					// Go through all of the node's pair's connections
					for(; nodeConnection; nodeConnection = nodeConnection->previousNodeConnectionLink) {
					
						// Check if the connected node's pair wasn't already visited
						if(!cuckatooVVisitedNodePairs.contains((nodeConnection + 1)->node >> 1)) {
						
							// Check if cycle is complete
							if(((nodeConnection + 1)->node ^ 1) == cuckatooRootNode) {
							
								// Check if cycle is a solution
								if(cycleSize == SOLUTION_SIZE - 1) {
								
									// Get solution from visited nodes
									cuckatooUVisitedNodePairs.getValues(solution);
									cuckatooVVisitedNodePairs.getValues(&solution[SOLUTION_SIZE / 2]);
									solution[SOLUTION_SIZE - 1] = (nodeConnection + 1)->edgeIndex;
									
									// Sort solution in ascending order
									sort(solution, solution + SOLUTION_SIZE);
									
									// Return true
									return true;
								}
							}
							
							// Otherwise check if cycle could be as solution
							else if(cycleSize != SOLUTION_SIZE - 1) {
							
								// Check if the connected node has a pair
								if(cuckatooVNewestNodeConnections.contains((nodeConnection + 1)->node ^ 1)) {
								
									// Check if solution was found at the connected node's pair
									if(searchNodeConnectionsForCuckatooSolutionSecondPartition(cycleSize + 1, (nodeConnection + 1)->node ^ 1, &(nodeConnection + 1)->edgeIndex)) {
									
										// Get solution from visited nodes
										cuckatooUVisitedNodePairs.getValues(solution);
										cuckatooVVisitedNodePairs.getValues(&solution[SOLUTION_SIZE / 2]);
										
										// Sort solution in ascending order
										sort(solution, solution + SOLUTION_SIZE);
										
										// Return true
										return true;
									}
								}
							}
						}
					}
					
					// Break
					break;
				}
				
				// Go to node's pair opposite end and get its edge index
				index = &(nodeConnection + 1)->edgeIndex;
				node = (nodeConnection + 1)->node;
				
				// Check if node pair was already visited
				if(cuckatooVVisitedNodePairs.contains(node >> 1)) {
				
					// Break
					break;
				}
				
				// Check if cycle is complete
				if((node ^ 1) == cuckatooRootNode) {
				
					// Check if cycle is a solution
					if(cycleSize == SOLUTION_SIZE - 1) {
					
						// Get solution from visited nodes
						cuckatooUVisitedNodePairs.getValues(solution);
						cuckatooVVisitedNodePairs.getValues(&solution[SOLUTION_SIZE / 2]);
						solution[SOLUTION_SIZE - 1] = *index;
						
						// Sort solution in ascending order
						sort(solution, solution + SOLUTION_SIZE);
						
						// Return true
						return true;
					}
					
					// Break
					break;
				}
				
				// Check if cycle isn't a solution
				if(cycleSize == SOLUTION_SIZE - 1) {
				
					// Break
					break;
				}
				
				// Check if node doesn't have a pair
				if(!cuckatooVNewestNodeConnections.contains(node ^ 1)) {
				
					// break
					break;
				}
				
				// Set that node pair has been visited
				cuckatooVVisitedNodePairs.setUnique(node >> 1, index);
				
				// Check if node's pair has more than one connection
				nodeConnection = cuckatooVNewestNodeConnections.get(node ^ 1);
				if(nodeConnection->previousNodeConnectionLink) {
				
					// Go through all of the node's pair's connections
					for(; nodeConnection; nodeConnection = nodeConnection->previousNodeConnectionLink) {
					
						// Check if the connected node has a pair
						if(cuckatooUNewestNodeConnections.contains((nodeConnection - 1)->node ^ 1)) {
						
							// Check if the connected node's pair wasn't already visited
							if(!cuckatooUVisitedNodePairs.contains((nodeConnection - 1)->node >> 1)) {
							
								// Check if solution was found at the connected node's pair
								if(searchNodeConnectionsForCuckatooSolutionFirstPartition(cycleSize + 2, (nodeConnection - 1)->node ^ 1, &(nodeConnection - 1)->edgeIndex)) {
								
									// Get solution from visited nodes
									cuckatooUVisitedNodePairs.getValues(solution);
									cuckatooVVisitedNodePairs.getValues(&solution[SOLUTION_SIZE / 2]);
									
									// Sort solution in ascending order
									sort(solution, solution + SOLUTION_SIZE);
									
									// Return true
									return true;
								}
							}
						}
					}
					
					// Break
					break;
				}
				
				// Go to node's pair opposite end and get its edge index
				index = &(nodeConnection - 1)->edgeIndex;
				node = (nodeConnection - 1)->node;
				
				// Check if node pair was already visited
				if(cuckatooUVisitedNodePairs.contains(node >> 1)) {
				
					// Break
					break;
				}
				
				// Check if node doesn't have a pair
				if(!cuckatooUNewestNodeConnections.contains(node ^ 1)) {
				
					// break
					break;
				}
			}
		}
	}
	
	// Return false
	return false;
}

// Search node connections for cuckatoo solution first partition
bool searchNodeConnectionsForCuckatooSolutionFirstPartition(const uint_fast8_t cycleSize, const uint32_t node, const uint32_t *index) noexcept {

	// Set that node pair has been visited
	const uint32_t visitedNodePairIndex = cuckatooUVisitedNodePairs.setUniqueAndGetIndex(node >> 1, index);
	
	// Go through all of the node's connections
	for(const CuckatooNodeConnectionsLink *nodeConnection = cuckatooUNewestNodeConnections.get(node); nodeConnection; nodeConnection = nodeConnection->previousNodeConnectionLink) {
	
		// Check if the connected node's pair wasn't already visited
		if(!cuckatooVVisitedNodePairs.contains((nodeConnection + 1)->node >> 1)) {
		
			// Check if cycle is complete
			if(((nodeConnection + 1)->node ^ 1) == cuckatooRootNode) {
			
				// Check if cycle is a solution
				if(cycleSize == SOLUTION_SIZE - 1) {
				
					// Set that the connected node's pair has been visited
					cuckatooVVisitedNodePairs.setUnique((nodeConnection + 1)->node >> 1, &(nodeConnection + 1)->edgeIndex);
					
					// Return true
					return true;
				}
			}
			
			// Otherwise check if cycle could be as solution
			else if(cycleSize != SOLUTION_SIZE - 1) {
			
				// Check if the connected node has a pair
				if(cuckatooVNewestNodeConnections.contains((nodeConnection + 1)->node ^ 1)) {
				
					// Check if solution was found at the connected node's pair
					if(searchNodeConnectionsForCuckatooSolutionSecondPartition(cycleSize + 1, (nodeConnection + 1)->node ^ 1, &(nodeConnection + 1)->edgeIndex)) {
					
						// Return true
						return true;
					}
				}
			}
		}
	}
	
	// Set that node pair hasn't been visited
	cuckatooUVisitedNodePairs.removeMostRecentSetUique(visitedNodePairIndex);
	
	// Return false
	return false;
}

// Search node connections for cuckatoo solution second partition
bool searchNodeConnectionsForCuckatooSolutionSecondPartition(const uint_fast8_t cycleSize, const uint32_t node, const uint32_t *index) noexcept {
	
	// Set that node pair has been visited
	const uint32_t visitedNodePairIndex = cuckatooVVisitedNodePairs.setUniqueAndGetIndex(node >> 1, index);
	
	// Go through all of the node's connections
	for(const CuckatooNodeConnectionsLink *nodeConnection = cuckatooVNewestNodeConnections.get(node); nodeConnection; nodeConnection = nodeConnection->previousNodeConnectionLink) {
	
		// Check if the connected node has a pair
		if(cuckatooUNewestNodeConnections.contains((nodeConnection - 1)->node ^ 1)) {
		
			// Check if the connected node's pair wasn't already visited
			if(!cuckatooUVisitedNodePairs.contains((nodeConnection - 1)->node >> 1)) {
			
				// Check if solution was found at the connected node's pair
				if(searchNodeConnectionsForCuckatooSolutionFirstPartition(cycleSize + 1, (nodeConnection - 1)->node ^ 1, &(nodeConnection - 1)->edgeIndex)) {
				
					// Return true
					return true;
				}
			}
		}
	}
	
	// Set that node pair hasn't been visited
	cuckatooVVisitedNodePairs.removeMostRecentSetUique(visitedNodePairIndex);
	
	// Return false
	return false;
}


#endif
