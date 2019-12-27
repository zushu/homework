import sys
import random

filepath = sys.argv[1]

#fp = open(filepath)

# for Value iteration or Q-learning
alg = ''
theta = -1
gamma = -1
M = -1
N = -1
k = -1
lst_obstacles = []
l = -1
lst_pitfalls = []
goal = (-1, -1)
r_regular = 0
r_obstacle = 0
r_pitfall = 0
r_goal = 0

# for Q-learning
num_episodes = -1
alpha = -1
epsilon = -1


# READING THE INPUT FILE  
with open(filepath) as fp:
    line = fp.readline().split()[0]

    if line == 'V':
        #print 'here'
        alg = line
        theta = float(fp.readline().split()[0])
        gamma = float(fp.readline().split()[0])
        M, N = [int(item) for item in fp.readline().split()]
        k = int(fp.readline().split()[0])
        k_copy = k
        while k_copy > 0:
            coord_tuple = tuple([int(item) for item in fp.readline().split()])
            lst_obstacles.append(coord_tuple)
            k_copy = k_copy - 1
        
        l = int(fp.readline().split()[0])
        l_copy = l
        while l_copy > 0:
            coord_tuple = tuple([int(item) for item in fp.readline().split()])
            lst_pitfalls.append(coord_tuple)
            l_copy = l_copy - 1

        goal = tuple([int(item) for item in fp.readline().split()])
        r_regular, r_obstacle, r_pitfall, r_goal = [int(item) for item in fp.readline().split()]


    elif line == 'Q':
        alg = line
        num_episodes = int(fp.readline().split()[0])
        alpha = float(fp.readline().split()[0])
        gamma = float(fp.readline().split()[0])
        epsilon = float(fp.readline().split()[0])
        M, N = [int(item) for item in fp.readline().split()]
        k = int(fp.readline().split()[0])
        k_copy = k
        while k_copy > 0:
            coord_tuple = tuple([int(item) for item in fp.readline().split()])
            lst_obstacles.append(coord_tuple)
            k_copy = k_copy - 1

        l = int(fp.readline().split()[0])
        l_copy = l
        while l_copy > 0:
            coord_tuple = tuple([int(item) for item in fp.readline().split()])
            lst_pitfalls.append(coord_tuple)
            l_copy = l_copy - 1

        goal = tuple([int(item) for item in fp.readline().split()])
        r_regular, r_obstacle, r_pitfall, r_goal = [int(item) for item in fp.readline().split()]


board = [['R' for x in range(M)] for y in range(N)]
for x, y in lst_obstacles:
    board[x-1][y-1] = 'O'
for x, y in lst_pitfalls:
    board[x-1][y-1] = 'P'

board[goal[0] - 1][goal[1] - 1] = 'G'

for line in board:
    print line, '\n'