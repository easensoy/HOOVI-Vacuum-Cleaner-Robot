# WARMING

The two graph-traversal algorithms, such as Breadth-First Search and Depth-First Search, are used to find the shortest path between the current location of the robot and the charging station.

# Breadth-First Search

Breadth first search is a graph traversal algorithm that starts traversing the graph from root node or initial node and traverse all the neighboring nodes. Then, it selects the closest node and visit all the unvisited nodes. The algorithm uses the same process for each of the closest node until it finds the goal or it reaches to destination.

# Depth-First Search

Depth first search (DFS) algorithm or searching technique starts with the root node of the graph G, and then travel to deeper and deeper until we find the goal node or the node which has no children by visiting different node of the tree.

# DFS vs BFS
![image](https://user-images.githubusercontent.com/76905667/182620015-ba53b7ad-3e95-4cc6-afd8-8dda643bfe0f.png)



# HOOVI-Vacuum-Cleaner-Robot
The floor plan

![image](https://user-images.githubusercontent.com/76905667/180611060-5617e08b-da6d-43ef-a1d0-ab6ee304090d.png)

## User Interface

The program has a text-based user interface (UI) which appears in a terminal or command window (depending on operating system) when the program is executed. All input and output takes place through that interface. There are multiple points in the program where the user is prompted to choose between either two or three options.

The user is prompted to choose from three options, by entering “0”, “1”, or “2”.User input is read into a C string (char array) using the scanf function. This approach was chosen because when more than a single character is entered with the getchar function, the extraneous characters can be incorrectly picked up by future calls to getchar. For example, if the user enters “12”, then the leading ‘1' character will be taken as the input to the current call to the getchar function, and the ‘2’ will be taken as the input to the next call to getchar, which is not the desired behaviour. When working with the input as a string, “12” need not be incorrectly accepted as a valid input.

When the user enters a string, the string is tested for equality with the three options, one after another, until either the input has been matched to one of the options or has been determined to be invalid. The C standard library strcmp function is used to compare the input strings with the options. This function returns an integer, which is 0 if the two strings are equal, and which is either greater than or less than 0 if the strings are not equal, depending on an alphabetical comparison
of the first non-matching character between the two strings. If we are only interested in whether or not the two strings are equal, and not in how they might be alphabetically ordered, then we can treat these returns as true/false values. Because strcmp returns 0 (i.e. false) when the strings are equal, and something which is not 0 (i.e. true, as all non-zero integers are treated as true in conditional expressions) when they are unequal, then we can use a logical inversion to make our conditions intuitive, e.g.

## Program Structure

HOOVI is an automatic vacuum cleaner that works in a 3-storey office building. The lifts are available to travel between these 3 floors. There is a lobby on each floor of the building, and in addition, there are 12 reception and 12 office rooms on each floor. HOOVI draws its power from the battery and can only be charged in the ground floor charging room when the battery runs out. When HOOVI's battery level is critically low, it should go to the charging room to recharge and then resume its duty. This means that HOOVI could be any room on any floor when it must return to the charging room. The algorithm in this project allows the user to find the shortest path from any room in the building to the charging room.

When the search algorithm finds a solution (i.e. a valid path between the given initial and goal locations), the solution is printed to screen using the printSolution function. This function has a simple purpose but is worth commenting on because it achieves that purpose using recursion, i.e. this function calls itself. It is also worth noting that a recursive solution was not the only option here, and the same behaviour could easily be achieved using a while loop. The recursive approach can lead to compact and neat code, but has potentially severe pitfalls.

One issue with recursion, which fortunately does not apply to this program, is related to memory usage. If any variables are created in a recursive function, then all the variables for all recursive calls to the function may be held in memory at once, which has the potential to result in stack overflow.

The more obvious danger of a function calling itself is that this can result in behaviour which is equivalent to an infinite loop. For this reason, every recursive function must have a base (sometimes also referred to as the halting, or stopping) case - there must be a condition which leads to the recursive function call if false, or to returning from the function if true. In this function, the recursive calls traverse the solution path from the goal location back towards the initial location. As the initial location is stored in a node which is the root of the search tree, and therefore has no parent node, we can use this to define our base case. Every time printSolution is called it is passed a “state”, which is really a node on the tree. If that node has a parent, then the function is called recursively with that parent, and if not, then the function completes and returns without making a recursive call. 

Each time printSolution is called, the location corresponding to the passed-in state (node) is printed to the screen. For all nodes which don’t correspond to the goal location, the location name is printed, followed by an arrow “—->” to indicate the direction of travel, if the current location is not the goal. The last interesting detail worth pointing out here is that the location is not printed until after the recursive function call. What this means is that although the initial location is not reached until the last recursive call is made, it is actually printed first, before the function returns to the prior recursive call (if there was one), which then prints its location, and so on until the goal location is printed last.

To explain the possible actions of HOOVI, a function generateSuccessor is defined that contains a set of operators. This function is a transform function on a state representation that transforms HOOVI's inter-floor and in-floor states into other states. The generateSuccessor which was created function is a way to define the accessibility relationship between states.

The Binary Tree

![tree](https://user-images.githubusercontent.com/76905667/182643316-d79076d7-7872-45b3-a173-85d5d3771d67.png)


![1](https://user-images.githubusercontent.com/76905667/180611167-71336fbe-f399-46f1-ab18-385b64bd11cf.png)
![2](https://user-images.githubusercontent.com/76905667/180611170-72c2e948-b274-4816-8019-00f7b1023a3f.png)

# The flowchart showing the logic of the main function

![image](https://user-images.githubusercontent.com/76905667/182635118-0d538cc8-f5d8-45fc-b744-9750fecedbff.png)
