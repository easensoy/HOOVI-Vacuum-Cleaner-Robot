#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// It can be useful to use struct to define a state representation
// because we can bundle up several variables in one state
typedef struct
{
   // your state rep here
   char letter;
   int floor;
   int number;
   int parent;
} state;

// function prototypes
void addToFringe(char, int, int, int);
int addToVisited(state);
int equalStates(state, state);
void generateSuccessors(state,int);
int hasBeenVisited(state);
int isGoalState(state);
void printSolution(state, state);
state pop();
// state popDFS();
void push(state);
int search(state, state, int, int);
state getRandomState();
void printQueue();
void printState(state, int);
void testSuccessors();
void clean();
void roomToRoom();
state inputRoom();
void setState(state*, char, int, int, int);
void tests();
void testGoalState();
int setSearchType();
void testCompare();
void setVerbatim();

// Variables at global scope
// ***
// Define here queue arrays (fringe, visited) and
// their pointer integers (front, rear)
state queue[1000]   = {};   // This is the FIFO waiting queue - we put all states yet to be examined in here
state visited[1000] = {};   // This is the visited states list - we put all states we have already examined in here
int vfront = -1; // next pos in the visited list
int front = 0; // front pos of the queue
int rear = -1; // rear pos of the queue
int stateCount = 0; // amount of states in the waiting list
int searchCost = 0; // a counter to measure the number of search iterations
int verbatim = 0; // a flag to turn detailed messages on or off - I only make this global to avoid changing the given functions more than necessary

/*

	Main function. I like to keep this function as simple and clear as possible,
	and make use of additional functions for the majority of the implmentation.
	
	In this case, I only use main to allow the use to select the highest-level
	options, and then the program is directed to functions corresponding to each.
	
	Note that here, as elsewhere, I always read user inputs into strings rather
	than chars. I found it convenient and clean to do things this way, but other
	approaches were fine too.

*/
int main()
{
	// seed pseudo-random number generator
	srand((unsigned int)time(NULL));
	
	// present user with options
	printf("Would you like to find a path from room to room (0), to clean all rooms (1), or run a test (2)?\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		// check to see whether user input matches one of the options
		if(!strcmp("0", buffer))
		{
			// search for route from location to location
			roomToRoom();
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			// generate cleaning routine
			clean();
			selected = 1;
		}
		else if(!strcmp("2", buffer))
		{
			// run a test
			tests();
			selected = 1;
		}
		else
		{
		  	printf("Invalid input. Please try again.\n");
		}
	}
	
	getchar();
	return 0;
}

/*

	There are many places in my code where I declare and initialise, or assign to,
	states in different places. This being the case, it is convenient to have this
	function for assigning values to the state members, as then everywhere that I
	want to assign to a state I only need write a single line of code to call this
	function.
	
	Note that I am using a pointer to a state here, rather than creating a new
	state, and for that reason need to use "->" rather than "." to access the
	state's members.

*/
void setState(state* s, char letter, int floor, int number, int parent)
{
	s->letter = letter;
	s->floor = floor;
	s->number = number;
	s->parent = parent;
}

/*

	A function for randomly generating valid states. Chiefly used for tests, but
	once I had it I thought why not add it to the options for finding paths from
	one location to another.

*/
state getRandomState()
{
	// valid letters
  	char letters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'K', 'L', 'M', 'N', 'O', 'P', 'Q'}; // 15
  	// randomly select room letter
  	int ind = rand() % 15;
  	char letter = letters[ind];

  	// randomly select floor
  	int floor = 1 + (rand() % 3);
  	// charger is only on ground floor, so override randomly selected floor if Q was selected above
  	if(letter == 'Q') floor = 1;

  	// randomly generate room number
  	int room = 1 + (rand() % 2);
  	// for lift, lobby and charging room, room number should always be 1
  	if((letter == 'O') || (letter == 'P') || (letter == 'Q'))
  	{
    	room = 1;
  	}

  	// create and return state
  	state s = {letter, floor, room, -1};
  	return s;
}

/*

	A simple function for making it easy to print out states from anywhere in the
	program.
	
	The parameters passed to the function are the state and the number of newlines
	to print after it - this will typically be 1, but I wanted to allow for the
	possibility of it being some other number, such as 0, e.g. if mutiple states
	are printed on a single line.

*/
void printState(state s, int newlines)
{
  	printf("%c.%d.%d,%d", s.letter, s.floor, s.number, s.parent);
  	for(int i = 0; i < newlines; i++) printf("\n");
}
	
/*

	A function for printing out the contents of the queue.

*/
void printQueue()
{
	printf("Queue: {");
	for(int i = front; i <= rear; i++)
	{
		printState(queue[i], 0);
		if(i < rear) printf(", ");
	}
	printf("}\n");
}

/*

	A function for allowing the user to select a location.
	
	I put this in its own function because I use it more than once, in particular
	for allowing the user to choose both initial location and goal location.
	
	There are two other reasons for giving this its own function:
	
	1. The amount of code here. If this was just one part of a function, that
	function would probably be very long.
	2. The rule of thumb that a function should do just one thing.
	
	There are some places in this function where a series of if statments could
	be combined into a single one, but I split them up for readability.

*/
state inputRoom()
{
	// declare state which will be set and later be returned
	state s;
	
	// tell user how to make inputs for selecting different types of location
	printf("\n---Please enter a location---\n");
	printf("For a lift, please enter \"O.[floor]\"\n");
	printf("For a lobby, please enter \"P.[floor]\"\n");
	printf("For the charging room, please enter \"Q\"\n");
	printf("For an office or reception, please enter \"[letter].[floor].[number]\"\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		// check to see whether user input matches one of the options
		if(!strcmp("Q", buffer)) // correct input for charging room?
		{
			setState(&s, 'Q', 1, 1, -1);
			selected = 1;
		}
		else if(strlen(buffer) == 3) // check length is valid for lifts and lobbies
		{
			// check first two chars are valid
			if(((buffer[0] == 'O') || (buffer[0] == 'P')) && (buffer[1] == '.'))
			{
				// check valid floor is specified
				if((buffer[2] >= '1') && (buffer[2] <= '3'))
				{
					setState(&s, buffer[0], buffer[2]-48, 1, -1);
					selected = 1;
				}
			}
		}
		else if(strlen(buffer) == 5) // offices and receptions
		{
			// check whether first cahr is valid for offices and receptions
			if(((buffer[0] >= 'A') && (buffer[0] <= 'H')) || ((buffer[0] >= 'K') && (buffer[0] <= 'N')))
			{
				// check full stops are correctly placed
				if((buffer[1] == '.') && (buffer[3] == '.'))
				{
					// check floor and room numbers are both valid
					if((buffer[2] >= '1') && (buffer[2] <= '3') && (buffer[4] >= '1') && (buffer[4] <= '2'))
					{
						setState(&s, buffer[0], buffer[2]-48, buffer[4]-48, -1);
				    	selected = 1;
					}
				}
			}
		}
		
		// if a valid selection was not made, prompt user to try again
		if(!selected)
		{
		  	printf("Invalid input. Please try again.\n");
		}
	}
	
	// print selected state to terminal
	printf("Selected state is: ");
	printState(s, 1);
	
	// return state
	return s;
}

/*

	A function for finding a path from location to location. The user can select
	whether to use BFS or DFS to search, and whether to use default, randomly set,
	or user-specified initial and goal locations.
	
	** I have deliberately not bothered to implement all of the default initial
	locations given in the assignment brief, as I have more than enough examples
	of option selection in this program, and it is trivial to simply add more of
	the same. **

*/
void roomToRoom()
{
	// set default initial and goal states
	state initialState = {'A', 1, 1, -1}; // initial state to start from
	state goalState = {'Q', 1, 1, -1}; // only the Q matters for the charging room
	
	// mode is used to select between BFS (0) or DFS (1)
	int mode = setSearchType();
	setVerbatim();
	
	// ask user to choose how to set initial and goal locations
	printf("Would you like to use default start and end points (0), random ones (1), or enter your own (2)?\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		// check to see whether user input matches one of the options
		if(!strcmp("0", buffer))
		{
			// continue with default locations
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			// set locations randomly
			initialState = getRandomState();
			goalState = getRandomState();
			selected = 1;
		}
		else if(!strcmp("2", buffer))
		{
			// ask user to specify locations
			initialState = inputRoom();
			goalState = inputRoom();
			selected = 1;
		}
		else
		{
			printf("Invalid input. Please try again.\n");
		}
	}
	
	// display initial and goal states
	printf("\nInitial state: ");
	printState(initialState, 1);
	printf("Goal state: ");
	printState(goalState, 1);
	// pause for user to read initial and goal states
	printf("\nPress enter to begin search\n");
	getchar();  // a minor bug makes it necessary for me to call getchar twice
	getchar();  //  - it seems a stray newline input (from the series of calls to scanf above?)
	        	//    is somehow making the program skip over the first call to getchar
	
	// call search function with given initial and goal states, and search mode
	search(initialState, goalState, mode, 1);
}

/*

	A function for making the robot drive around all rooms on an efficient
	cleaning routine.
	
	* The robot begins in the charging room.
	* It cleans one floor at a time.
	* It repeatedly calls the search function, which takes initial and goal states
	as parameters.
	
	Every time the search function is called, the current location in used as the
	initial state. The goal location is always an office (until the very end, when
	the charging room  is made the goal, so that the robot returns home), as to get
	to the offices, the robot must pass through all lifts, lobbies and receptions.

*/
void clean()
{
	// select BFS or DFS
	int mode = setSearchType();
	// verbatim mode is more trouble than it's worth here, given how many searches are involved!
	// setVerbatim();
	
	// start in the charging room
	state initialState = {'Q', 1, 1, -1};
	// declare but assign later
	state goalState;
	// for all floors
	for(int i = 1; i <= 3; i++)
	{
		// always begin cleaning a floor by heading to office A
		char c = 'A';
		// set the goal state
		setState(&goalState, c, i, 2, -1);
		// run the search to get the path from current to goal location
		search(initialState, goalState, mode, 1);
		 // for all offices
		while(c < 'N')
		{
			// the previous goal location is now the current location, so make it the
			// initial state for the next search
			initialState = goalState;
			// set letter of next office to go to
			if(c == 'H') c = 'K'; // jump over the gap between H and K
			else c++; // otherwise, just increment the letter by 1
			// set goal state
			setState(&goalState, c, i, 2, -1);
			// run the search to get the path from current to goal location
			search(initialState, goalState, mode, 1);
		}
		// the previous goal location is now the current location, so make it the
		// initial state for the next search
		// - we do this again here because we may be about to go to the next floor
		initialState = goalState;
	}
	// return to charging room
	setState(&goalState, 'Q', 1, 1, -1);
	search(initialState, goalState, mode, 1);
}

/*

	A function for selecting and running a test.
	
	There are 3 tests to choose from. More could certainly be added, but I think
	this is a sufficient number of examples.

*/
void tests()
{
	// present user with options
	printf("Would you like to test the generateSuccessors function (0), the isGoalState function (1), or would you like to compare BFS and DFS (2)?\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		if(!strcmp("0", buffer))
		{
			// test the successor function
			testSuccessors();
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			// test the goal state function
			testGoalState();
			selected = 1;
		}
		else if(!strcmp("2", buffer))
		{
			// compare BFS and DFS
			testCompare();
			selected = 1;
		}
		else
		{
			printf("Invalid input. Please try again.\n");
		}
	}
}

/*

	A function for running a quick (but not complete) test of the isGoalState function.
	20 random states are generated and tested. There will normally, but not always,
	be at least one in there which is the goal (charging room).

*/
void testGoalState()
{
	printf("\n------Test isGoalState function------\n\n");
	// for 20 randomly generated states
	for(int i = 0; i < 20; i++)
	{
		// randomly generate a state
		state s = getRandomState();
		printf("This is ");
		
		// check if state is goal
		if(!isGoalState(s)) printf("not ");
		printf("the goal state: ");
		printState(s, 1);
  	}
}

/*

	A function for testing the generateSuccessors function.
	20 random states are generated and tested.
	For each of those states, the list of successors the generateSuccessors
	function produces is displayed, so that it can be manually checked for
	correctness.

*/
void testSuccessors()
{
	printf("\n------Test generateSuccessors function------\n\n");
	// for 20 randomly generated states
	for(int i = 0; i < 20; i++)
	{
		// randomly generate a state
		state s = getRandomState();
		printf("\nState: ");
		printState(s, 1);
		
		// generate successors
		generateSuccessors(s, -1);
		printf("Sucessors: ");
		
		// print successors - as we alway start with an empty queue, the queue will
		// only contain all of the successors at this point
		printQueue();
		
		// reset queue for next iteration
		front = 0; // front pos of the queue
		rear = -1; // rear pos of the queue
	}
}

/*

	A function for a comparison of BFS and DFS searches.
	
	100 times, initial and goal states are generated, and then BFS and DFS searches
	are both performed for them
	
	The search costs are then compared, by dividing the BFS cost by the DFS cost.
	For each comparison, the result is printed, and the average result is also
	printed at the end. BFS is consistently the user with this number of comparisons.
	For a more accurate comparison, more than 100 tests would be preferable.
	
	Note: this only works because there is a single solution for any given pair
	of states in this problem space. If there were multiple solutions, a similar
	test would still be possible, but less easy to achieve.

*/
void testCompare()
{
	printf("\n------Compare BFS and DFS------\n\n");
	printf("Displayed ratio is (BFS cost)/(DFS cost)\n");
	printf("If it is greater than 1, then BFS is taking longer than DFS\n");
	printf("If it is less than 1, then DFS is taking longer\n\n");
	printf("Would you like to see solutions (0), or not (1)?\n");
	
	// declare and assign later
	int showSolutions;
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		if(!strcmp("0", buffer))
		{
			showSolutions = 1;
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			showSolutions = 0;
			selected = 1;
		}
		else
		{
			printf("Invalid input. Please try again.\n");
		}
	}
	
	// number of times to compare BFS and DFS - it would be good to let the user input this
	int n = 100;
	// a running total, used for calculating the average later
	float sum = 0;
	for(int i = 0; i < n; i++)
	{
		// randomly select states
		state initialState = getRandomState();
		state goalState = getRandomState();
		
		// search actually returns an int, but directly promoting the returns to
		// floats here makes it easier to compute the ratio below
		float bfsCost = search(initialState, goalState, 0, showSolutions);
		float dfsCost = search(initialState, goalState, 1, showSolutions);
		
		// display costs
		printf("BFS cost: %f, DFS cost: %f\n", bfsCost, dfsCost);
		
		// set ratio to 1: if DFS cost = BFS cost = 0, then this will not be changed
		float ratio = 1;
		// only make this calculation if dfsCost != 0, to avoid nan
		if(dfsCost)
		{
			ratio = bfsCost / dfsCost;
		}
		
		// add current ratio to running total
		sum += ratio;
		// print current ratio
		printf("Ratio %d: %f\n", i, ratio);
	}
	// compute and print average ratio
	printf("Average ratio = %f\n\n", sum / n);
}

/*

	A simple function for setting the search type: BFS or DFS.

*/
int setSearchType()
{
	// variable for specifying search type (or mode)
	int mode;
	
	printf("Would you like to use BFS (0), or DFS (1)?\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		if(!strcmp("0", buffer))
		{
			mode = 0; // BFS
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			mode = 1; // DFS
			selected = 1;
		}
		else
		{
			printf("Invalid input. Please try again.\n");
		}
	}
	return mode;
}

/*

  A simple function for setting whether or not detailed messages are printed to the screen.

*/
void setVerbatim()
{
	printf("Would you like to see detailed information while searching (0), or not (1)?\n");
	
	// buffer for user input
	char buffer[100];
	
	// int to use as boolean flag which indicates whether or not user has made a valid selection
	int selected = 0;
	// loop until user makes a valid selection
	while(!selected)
	{
		// read user input into buffer
		scanf("%s", buffer);
		
		if(!strcmp("0", buffer))
		{
			verbatim = 1;
			selected = 1;
		}
		else if(!strcmp("1", buffer))
		{
			selected = 1;
		}
		else
		{
			printf("Invalid input. Please try again.\n");
		}
	}
}

/*

	A function for searching for a route from one location to another.
	
	This is in its own function for two reasons:
		1. I like to "unclutter" the main method - this is not absolutely necessary, but is my habit.
		2. This code is used for cleaning as well as for simple searches (and one of the implemented tests)
		- putting it in its own function avoids code duplication.
	
	The initialState and goalState parameters specify the start and end points of
	the route to find.
	
	The mode variable is used to select between using BFS (0) or DFS (1) to perform the search.
	
	The displaySolution variable is used to specify whether or not the solution is printed to the screen.
	This is because I might not always want to see the solutions, e.g. when running the testCompare function.

*/
int search(state initialState, state goalState, int mode, int displaySolution)
{
	int cost = searchCost;
	
	state s; 					// variable to hold the current state that we want to explore during search
	int parentIndex = 0;			// array index of the current state in the "visited" list
	// printf("*** WELCOME ***\n");
	
	// printQueue();
	
	push(initialState);				// add initial state to the "waiting" queue
	
	// printQueue();
	
	// Search - we are trying to explore states as long as there are any left in the queue
	if(verbatim) printf("\nI'm searching...\n");
	
	//
	while(stateCount > 0)
	{
		// GET NEXT STATE
		// get a state from the queue
		s = pop(mode);
		
		parentIndex = addToVisited(s);	// add this state to the visited list and retrieve storage index
		
		// GOAL TEST
		if(equalStates(s, goalState))
		{
			// if the current state is the goal, then print the solution
			if(displaySolution)
			{
				printf("\nI found a solution (%d states explored): ", searchCost);
				printSolution(s, goalState);
				printf("\n");
			}
			
			if(verbatim)
			{
				// Wait for key press
				printf("\nPress enter to continue\n");
				getchar();
				printf("\nI'm searching...\n");
			}
			
			cost = searchCost; // THIS RELIES ON THE KNOWLEDGE THAT THERE IS ONLY ONE SOLUTION, AND WOULD NOT BE CORRECT FOR ALL PROBLEMS!!!
		}
		// if current state s is not the goal, then run successor function
		else
		{
			generateSuccessors(s, parentIndex);   // generate the children of s, and make them remember s as their parent
		}
		// increment search iterations counter
		searchCost++;
	}
	
	if(verbatim)
	{
		printf("\n\nNo further solutions found. Press enter to continue");
		getchar();
	}
	
	// reset queue and search cost 
	vfront = -1; // next pos in the visited list
	front = 0; // front pos of the queue
	rear = -1; // rear pos of the queue
	searchCost = 0; // a counter to measure the number of search iterations
	
	return cost;
}

/* CORE SEARCH FUNCTIONS
*
*  push and pop - control the queue operations that implement BFS as our search strategy
*  generateSuccessors - creates further states to explore according to the rules of the problem
*  isGoalState - checks if the current state is a goal state
*
*  Details below
*/

// push adds a state to the rear of the queue
void push(state s)
{
	rear++;          // increase rear index
	queue[rear] = s; // store s
	stateCount++;    // increase the count of states in the queue
}

// pop retrieves a state from the queue
state pop(int mode)
{
	// declare and assign later, so that we can return from this scope (i.e. not inside the if-else)
	state s;
	// check if there are items in the queue
	if(stateCount > 0)
	{
		if(!mode)
		{
			s = queue[front]; // get state at front index
			front++;		  // increase front index to point at the next state
			stateCount--; 	  // decrement state counter
			if(verbatim)
			{
				printf("\nPopping state: ");
				printState(s, 1);
			}
		}
		else
		{
			state s = queue[rear]; // get state at front index
			rear--;			       // increase front index to point at the next state
			stateCount--; 		   // decrement state counter
			return s;       	   // pass state back to the point of call
		}
	}
	else
	{
		// this else was not in the original code, which is not great 
		// - actually, this function should never be called if stateCount <= 0, but if we are going to check whether or not that is the case here, then we should also implement responses for both true and false outcomes
		// for this reason, I return a state with clearly invalid values - these values could be checked in the calling function if we wanted to implement error-checking
		state dummy = {'z', -2, -2, -2};
		s = dummy;
		printf("Something has gone wrong - returning error state");
	}
	return s; // pass state back to the point of call
}

// Takes a single state and that state's storage index in the visited list as input
// It then generates all valid successor states (children) of this state
// The contents of the successor function depend largely on the problem we are trying to solve
void generateSuccessors(state s, int p)
{
	if(verbatim) printf("Generating successors: \n");
	
	if(s.letter == 'O') // if currently in lift
	{
		// go up
		if(s.floor < 3)
		{
			addToFringe(s.letter, s.floor + 1, 1, p);
		}
		
		// go down
		if(s.floor > 1)
		{
			addToFringe(s.letter, s.floor - 1, 1, p);
		}
		
		// go to lobby
		addToFringe('P', s.floor, 1, p);
		
		// go to charging room
		if(s.floor == 1)
		{
		  addToFringe('Q', s.floor, 1, p);
		}
	}
	else if(s.letter == 'P') // if currently in a lobby
	{
		// go to receptions
		for(char c = 'A'; c <= 'H'; c++)
		{
			addToFringe(c, s.floor, 1, p);
		}
		for(char c = 'K'; c <= 'N'; c++)
		{
			addToFringe(c, s.floor, 1, p);
		}
		
		// go to lift
		addToFringe('O', s.floor, 1, p);
	}
	else if(s.letter == 'Q') // currently in the charging room
	{
		// go to lift
		addToFringe('O', s.floor, 1, p);
	}
	else // currently in either an office or a reception
	{
		if(s.number == 1) // in reception
		{
			// go to office
			addToFringe(s.letter, s.floor, 2, p);
			// go to lobby
			addToFringe('P', s.floor, 1, p);
		}
		else // in office
		{
			// go to reception
			addToFringe(s.letter, s.floor, 1, p);
		}
	}
}

// Takes a state as input and checks if this state is the goal state
// Returns 1 if this is so, and 0 if the state is not the goal
int isGoalState(state s)
{
	return s.letter == 'Q';
}

// OUTPUT
// printSolution calls itself recursively to backtrack to the root and print each state
// in sequence.
void printSolution(state s, state goalState)
{
	// Check if we reached the root state
	if(s.parent != -1) printSolution(visited[s.parent], goalState); // Recursive call to the parent of s
	
	if(s.letter == 'Q')
	{
		printf("Charging room");
	}
	else if(s.letter > 'N')
	{
		printf("Floor %d ", s.floor);
		if(s.letter == 'O')
		{
			printf("lift");
		}
		else if(s.letter == 'P')
		{
			printf("lobby");
		}
	}
	else
	{
		printf("%c.%d.%d", s.letter, s.floor, s.number);
	}
	
	// print arrows if not the goal (i.e., last) state
	if(!equalStates(s, goalState))	printf("--->");
}

// UTILITY FUNCTIONS

// addToVisited takes a state as an argument and adds it to the visited list
// returns the position in the list  at which the state was stored
// (we need this information for maintaining parent links)
int addToVisited(state s)
{
    vfront++;				// increase list index
    visited[vfront] = s;	// add state at index
    return vfront;			// return storage index for s
}

// equalStates takes two states as input and compares their internal variables a and b
// if both a and both b values are equal, this function will return 1, otherwise 0;
int equalStates(state s1, state s2)
{
	return ((s1.letter == s2.letter) && (s1.floor == s2.floor) && (s1.number == s2.number));
}

// hasBeenVisited takes a state as input and compares it to all states stored in the "visited" list
// returns 1 if the state has been visited before
// returns 0 if the state is new
int hasBeenVisited(state s)
{
	for(int i=0; i<vfront; i++)
	{
		if(equalStates(visited[i],s))
		{
			if(verbatim)
			{
				printf("State has already been visited: ");
				printState(s, 1);
			}
			return 1;
		}
	}
	return 0;
}

// addToFringe takes a state as input and checks if this state has not been explored yet
// If the state was not previously visited, then we recognise the state
// to be "at the fringe" of its parent and add it to the waiting queue
// otherwise, the function returns to the point of call without doing anything
void addToFringe(char letter, int floor, int number, int parent)
{
	state s = {letter, floor, number, parent};
	// check if s was visited before
	if(!hasBeenVisited(s))
	{
		push(s);	// if not, then add to queue
		
		if(verbatim)
		{
			printf("Adding state to queue: ");
			printState(s, 1);
			printQueue();
		}
	}
}
