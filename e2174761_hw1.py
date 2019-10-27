import copy

# TODO: transform into OO design

# global variables
# TODO: remove strings
method = input("algorithm to be used: ")
M = int(input("maximum total estimated cost: "))
k = int(input("board dimension: "))

board_init = [[0 for x in range(k)] for y in range(k)]
board_final = [[0 for x in range(k)] for y in range(k)]

for i in range(k):
    board_init[i] = list(map(str, input().split()))

for i in range(k):
    board_final[i] = list(map(str, input().split()))

print(board_init)
print()
print(board_final)

# TODO: add depth
class Node():
    def __init__(self, state, parent, f, g, h):
        self.state = state
        self.parent = parent
        # costs
        self.f = f
        self.g = g
        self.h = h
        self.children = []


def a_star():
    explored = []
    root = Node(board_init, False, 0, 0, 0)
    root.h = h(root.state)
    root.f = root.h + root.g
    frontier = [root]
    
    if frontier == []: return False
    
    while frontier != []:
        x = min(frontier, key = lambda node : node.f)
        if x.state == goal: return x
        add_x_to_explored = True
        for y in explored:
            if y.state == x.state and y.f <= x.f:
                add_x_to_explored = False
                break
        if add_x_to_explored == True:
            explored.append(x)
            x_children = expand(x)
            explored.extend(x_children)
        
# manhattan distance between two points
def manhattan(index1, index2):
    ind1_i, ind1_j = index1;
    ind2_i, ind2_j = index2;
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
    
lst1 = [Node(1, False, 2, 2, 0), Node(2, False, 3, 2, 1), Node(3, False, 5, 2, 3)]
print(min(lst1, key=lambda x: x.f).f)
    
