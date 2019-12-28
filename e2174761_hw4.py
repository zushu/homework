import sys
import random
#import copy

filepath = sys.argv[1]

#fp = open(filepath)

# for Value iteration or Q-learning
alg = ''
theta = -1  # convergence factor
gamma = -1 # discount factor
M = -1 # y-dimension
N = -1 # x-dimension
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
        r_regular, r_obstacle, r_pitfall, r_goal = [float(item) for item in fp.readline().split()]


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
        r_regular, r_obstacle, r_pitfall, r_goal = [float(item) for item in fp.readline().split()]


board = [['R' for i in range(M)] for j in range(N)]
for i, j in lst_obstacles:
    board[i-1][j-1] = 'O'
for i, j in lst_pitfalls:
    board[i-1][j-1] = 'P'

board[goal[0] - 1][goal[1] - 1] = 'G'

for line in board:
    print line, '\n'

rewards2 = {'R': r_regular, 'O': r_obstacle, 'P': r_pitfall, 'G': r_goal}
actions2 = {'N': (1, 0), 'S': (-1, 0), 'E': (0, 1), 'W': (0, -1)}


# VALUE ITERATION
def value_iteration(states, rewards, actions, gamma, theta):
    utilities = [[0 for i in range(M)] for j in range(N)]   # U
    utilities_2 = [[float('inf') for i in range(M)] for j in range(N)] # U'

    delta = float('inf') # maximum change in the utility of any state in an iteration

    while delta >= theta:
        print 'delta > theta'
        delta = 0
        utilities_2 = [item[:] for item in utilities] # deep copy
        #actions_state = actions
        for i in range(M):
            for j in range(N):
                # actions
                actions_state = {'N': (1, 0), 'S': (-1, 0), 'E': (0, 1), 'W': (0, -1)} # deep copy
                
                if i == 0 and 'S' in actions_state: 
                    actions_state.pop('S')
                if j == 0 and 'W' in actions_state:
                    actions_state.pop('W')
                if i == M - 1 and 'N' in actions_state:
                    actions_state.pop('N')
                if j == N - 1 and 'E' in actions_state:
                    actions_state.pop('E')

                #max_action 
                next_states_list = [utilities[i + a_i][j + a_j] for key, (a_i, a_j) in actions_state.iteritems()]

                Q_a = [ns for ns in next_states_list]
                for ind, next_state in enumerate(next_states_list):
                    Q_a[ind] = rewards[states[i][j]] + gamma * next_state
                    #utilities_2[i][j] = rewards[states[i][j]] + gamma * next_state
                utilities[i][j] = max(Q_a)

                if abs(utilities_2[i][j] - utilities[i][j]) > delta:
                    delta = abs(utilities_2[i][j] - utilities[i][j])
                #print 'delta', delta

    return utilities

#print rewards2
print 'gamma, theta: ', gamma, theta

for item in value_iteration(board, rewards2, actions2, gamma, theta):
    print item, '\n' 






