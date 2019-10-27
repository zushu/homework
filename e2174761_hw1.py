import copy

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


def a_star(state, goal, M, heuristics="manhattan"):
    explored = []
    frontier = [Node(state, False, 0)]
    
    if frontier == []: return False
    
    while frontier != []:
        x = min(frontier, key = lambda node : node.f)
        if x.state == goal: return x
        
def manhattan(index1, index2):
    ind1_i, ind1_j = index1;
    ind2_i, ind2_j = index2;
    return abs(ind1_i - ind2_i) + abs(ind1_j - ind2_j)

def expand(node):
    state = node.state
    children = []
    i, j = find_index(state, '_')
    return

# find index in 2d list
def find_index(lst, to_be_found):
    result = [(i, item.index(to_be_found)) for i, item in enumerate(lst) if to_be_found in item]
    if result != []:
        return result[0]

    return []
    
lst1 = [Node(1, False, 2, 2, 0), Node(2, False, 3, 2, 1), Node(3, False, 5, 2, 3)]
print(min(lst1, key=lambda x: x.f).f)
    
