import copy

# TODO: transform into OO design

# global variables
# TODO: remove strings
method = input()
M = int(input())
k = int(input())

board_init = [[0 for x in range(k)] for y in range(k)]
board_final = [[0 for x in range(k)] for y in range(k)]

for i in range(k):
    board_init[i] = list(map(str, input().split()))

for i in range(k):
    board_final[i] = list(map(str, input().split()))

# TODO: add depth
class Node():
    def __init__(self, state, parent, f, g, h):
        self.state = state
        self.parent = parent
        # costs
        self.f = f
        self.g = g
        self.h = h
        #self.tag = tag
        #self.children = []


def a_star(init_state=board_init, goal=board_final):
    explored = []
    root = Node(init_state, False, 0, 0, 0)
    root.h = h(root.state)
    root.f = root.h + root.g
    frontier = [root]
    """
    if frontier == []: 
        print("FAILURE")
        return False
    """
    
    while frontier != []:
        x = min(frontier, key = lambda node : node.f)
        """
        min_indices = [i for i, elem in enumerate(frontier)
                if elem.f == x.f]
        min_nodes = []
        for index_of_min in min_indices:
            min_nodes.append(frontier[index_of_min])

        sort_order = ["up", "down", "left", "right"]
        min_nodes = sorted(min_nodes, key = lambda node : sort_order.index(node.tag))

        #for temp in min_nodes:
        #    print_node(temp)

        x = min_nodes[0]
        """       

        if x.state == goal: 
            print("SUCCESS")
            path = get_path(x)
            for item in path:
                print_board(item)
            return x

        frontier.remove(x)
        explored.append(x)

        x_children = expand(x)
        for x_child in x_children:
            for y in frontier:
                if y.state == x_child.state and x_child.f < y.f:
                    frontier.remove(y)

            for y in explored:
                if y.state == x_child.state and x_child.f < y.f:
                    explored.remove(y)

            #if x_child not in frontier and x_child not in explored:
            add_x_child_to_frontier = True
            for y in frontier:
                if y.state == x_child.state:
                    add_x_child_to_frontier = False

            for y in explored:
                if y.state == x_child.state:
                    add_x_child_to_frontier = False

            if add_x_child_to_frontier and x_child.f < M:
                frontier.append(x_child)


        """
        add_x_to_explored = True
        for y in explored:
            if y.state == x.state and y.f <= x.f:
                add_x_to_explored = False
                break

        if add_x_to_explored == True:
            #print_board(x.state)
            explored.append(x)
            x_children = expand(x)
            frontier.extend(x_children)
            #frontier.reverse()
        """

    # if frontier is empty and no goal state is found
    print("FAILURE")
    return False

def ida_star(init_state=board_init, goal=board_final, f_max=M):
    root = Node(init_state, False, 0, 0, 0)
    root.h = h(root.state)
    root.f = root.h + root.g
    bound = root.h
    path = [root]

    while True:
        if bound > f_max: 
            print("FAILURE")
            return "NOT FOUND"
        t = limited_f_search(path, 0, goal, bound)
        if t == "FOUND" : 
            print("SUCCESS")
            for item in path:
                print_board(item.state)
            return (path, bound)
        if t == float("inf") : 
            print("FAILURE")
            return "NOT FOUND"
        if t > f_max:
            print("FAILURE")
            return "NOT FOUND"
        bound = t

def limited_f_search(path, g, goal, bound):
    node = path[-1]
    f = g + h(current_state=node.state)
    if f > bound:
        return f

    if node.state == goal: return "FOUND"

    minimum = float('inf')

    children = expand(node)
    for child in children:
        add_child_to_path = True
        for y in path:
            if child.state == y.state:
                add_child_to_path = False

        #if child in path:
        #    add_child_to_path = False

        if add_child_to_path:
            path.append(child)
            # 1 is total_manhattan(child.state, node.state)
            t = limited_f_search(path, g+1, goal, bound)
            if t == "FOUND" : return "FOUND"
            if t < minimum : minimum = t
            path.pop()
    
    return minimum
            



def get_path(node):
    path = [node.state]
    while node.parent:
        node = node.parent
        path.append(node.state)
    path.reverse()
    return path

# manhattan distance between two points
def manhattan(index1, index2):
    ind1_i, ind1_j = index1
    ind2_i, ind2_j = index2
    return abs(ind1_i - ind2_i) + abs(ind1_j - ind2_j)

# g function
# manhattan distance between two board configurations
def total_manhattan(board_before, board_after):
    total_distance = 0
    for i in range(k):
        for j in range(k):
            if board_before[i][j] == '_':
                continue
            index_after = find_index(board_after, board_before[i][j]) 
            total_distance = total_distance + manhattan((i, j), index_after)

    return total_distance
            
# heuristic function
def h(current_state):
    return total_manhattan(current_state, board_final)

# k: board dimension
def expand(node):
    state = node.state
    children = []
    i, j = find_index(state, '_')
    # up, down, left, right
    # up - move the tile below blank up
    if i != k-1 :
        new_state = copy.deepcopy(state)
        new_state[i][j], new_state[i+1][j] = new_state[i+1][j], new_state[i][j]
        # TODO: write a create child function
        child_node = Node(new_state, node, 0, 0, 0)
        child_node.g = node.g + 1
        child_node.h = h(child_node.state)
        child_node.f = child_node.g + child_node.h
        children.append(child_node)
        
    # down - move the tile above blank down
    if i != 0:
        new_state = copy.deepcopy(state)
        new_state[i-1][j], new_state[i][j] = new_state[i][j], new_state[i-1][j]
        child_node = Node(new_state, node, 0, 0, 0)
        child_node.g = node.g + 1
        child_node.h = h(child_node.state)
        child_node.f = child_node.g + child_node.h
        children.append(child_node)

    # left - move the tile to the right of blank to the left
    if j != k-1:
        new_state = copy.deepcopy(state)
        new_state[i][j], new_state[i][j+1] = new_state[i][j+1], new_state[i][j]
        child_node = Node(new_state, node, 0, 0, 0)
        child_node.g = node.g + 1
        child_node.h = h(child_node.state)
        child_node.f = child_node.g + child_node.h
        children.append(child_node)

    # right - move the tile to the left of blank to the right
    if j != 0:
        new_state = copy.deepcopy(state)
        new_state[i][j-1], new_state[i][j] = new_state[i][j], new_state[i][j-1]
        child_node = Node(new_state, node, 0, 0, 0)
        child_node.g = node.g + 1
        child_node.h = h(child_node.state)
        child_node.f = child_node.g + child_node.h
        children.append(child_node)

    return children

# find index in 2d list
def find_index(lst, to_be_found):
    result = [(i, item.index(to_be_found)) 
                for i, item in enumerate(lst) 
                    if to_be_found in item]

    if result != []:
        return result[0]

    return []

def print_board(board):
    print()
    for i in range(k):
        print(" ".join(board[i]))
    #print()

"""
def print_node(node):
    #print_board(node.state)
    if (node.parent == False):
        print("no parent")
    else:
        print_board(node.parent.state)
    print(node.f)
    print(node.g)
    print(node.h)
    #print(node.tag)
"""

if method == "A*":    
    a_star()
elif method == "IDA*":
    ida_star()
else:
    print("Method is not defined.")
