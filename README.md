# HOOVI-Vacuum-Cleaner-Robot
BFS &amp; DFS

![image](https://user-images.githubusercontent.com/76905667/180611060-5617e08b-da6d-43ef-a1d0-ab6ee304090d.png)

HOOVI is an automatic vacuum cleaner that works in a 3-storey office building. The lifts are available to travel between these 3 floors. There is a lobby on each floor of the building, and in addition, there are 12 reception and 12 office rooms on each floor. HOOVI draws its power from the battery and can only be charged in the ground floor charging room when the battery runs out. When HOOVI's battery level is critically low, it should go to the charging room to recharge and then resume its duty. This means that HOOVI could be any room on any floor when it must return to the charging room. The algorithm in this project allows the user to find the shortest path from any room in the building to the charging room.

ASCII codes was used to reference rooms when obtaining location from the user. Thus, while the room A corresponded to 65, the room N corresponded to 78. Further, the lobby and lift, corresponding to ASCII codes 73 and 74, was presented to the user as I and J respectively. In the 3-storey building, the remaining I (refers to 73) and J (refers to 74) were defined as lobby and lift, respectively, since these were no room names.

To explain the possible actions of HOOVI, a function generateSuccessor is defined that contains a set of operators. This function is a transform function on a state representation that transforms HOOVI's inter-floor and in-floor states into other states. The generateSuccessor which was created function is a way to define the accessibility relationship between states.

In this project, since the aim is to reach the charger room, there is an obligation to go down as fast as possible. Therefore, it would be more advantageous to use Depth-First Search (DFS) as it directly searches for the first floor. Breadth-First Search is at a disadvantage against DFS because it is based on horizontal search and FIFO (First in, First out).

It was used two major searching algorithms such as Breadth-First Search and Depth-First Search.

Breadth-First Search(BFS): Breadth-first search is a graph traversal algorithm that begins at the root node and explores all neighboring nodes.

Depth-First Search(DFS): The depth-first search (DFS) algorithm begins with the first node of the graph and proceeds to go deeper and deeper until we find the goal node or node with no children.

![1](https://user-images.githubusercontent.com/76905667/180611167-71336fbe-f399-46f1-ab18-385b64bd11cf.png)
![2](https://user-images.githubusercontent.com/76905667/180611170-72c2e948-b274-4816-8019-00f7b1023a3f.png)
