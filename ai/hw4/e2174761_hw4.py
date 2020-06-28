import sys
import random
#import copy

filepath = sys.argv[1]

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
        #M, N = [int(item) for item in fp.readline().split()]
        tmp = [int(item) for item in fp.readline().split()]
        M = tmp[0]
        N = tmp[1]
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
for j, i in lst_obstacles:
    board[i-1][j-1] = 'O'
for j, i in lst_pitfalls:
    board[i-1][j-1] = 'P'

board[goal[0] - 1][goal[1] - 1] = 'G'

rewards = {'R': r_regular, 'O': r_obstacle, 'P': r_pitfall, 'G': r_goal}
actions = {'N': (1, 0), 'S': (-1, 0), 'E': (0, 1), 'W': (0, -1)}


# VALUE ITERATION

def value_iteration(states=board, rewards=rewards, actions=actions, gamma=gamma, theta=theta):
    utilities = [[0 for i in range(M)] for j in range(N)]   # U
    utilities_2 = [[float('inf') for i in range(M)] for j in range(N)] # U'
    chosen_actions = [['0' for i in range(M)] for j in range(N)]

    delta = float('inf') # maximum change in the utility of any state in an iteration

    while delta >= theta:
        delta = 0
        utilities_2 = [item[:] for item in utilities] # deep copy
        for i in range(M):
            for j in range(N):
                # actions
                actions_state = {'N': (1, 0), 'S': (-1, 0), 'E': (0, 1), 'W': (0, -1)} # for deep copy
                
                if i == 0 and 'S' in actions_state: 
                    actions_state.pop('S')
                if j == 0 and 'W' in actions_state:
                    actions_state.pop('W')
                if i == M - 1 and 'N' in actions_state:
                    actions_state.pop('N')
                if j == N - 1 and 'E' in actions_state:
                    actions_state.pop('E')

                for key in actions_state.keys():
                    a_i, a_j = actions_state[key]
                    if states[i + a_i][j + a_j] == 'O':
                        actions_state.pop(key)

                #max_action 
                next_states_list = {key: utilities[i + a_i][j + a_j] for key, (a_i, a_j) in actions_state.iteritems()}
                #for key, (a_i, a_j) in actions_state.iteritems():
                #    if states[i + a_i][j + a_j] == 'O':
                #        next_states_list.pop(key)

                #if next_states_list != {}:

                Q_a = {key: val for key, val in next_states_list.iteritems()} # just for definition
                for key, next_state in next_states_list.iteritems():
                    Q_a[key] = rewards[states[i][j]] + gamma * next_state
                    #utilities_2[i][j] = rewards[states[i][j]] + gamma * next_state
                chosen_actions[i][j] = max(Q_a, key=Q_a.get)
                #utilities[i][j] = max(Q_a.values())
                utilities[i][j] = Q_a[chosen_actions[i][j]]

                if abs(utilities_2[i][j] - utilities[i][j]) > delta:
                    delta = abs(utilities_2[i][j] - utilities[i][j])
    
    output_list = [[' ' for i in range(M)] for j in range(N)]
    for i in range(M):
            for j in range(N):
                if chosen_actions[i][j] == 'N':
                    chosen_actions[i][j] = '0'
                elif chosen_actions[i][j] == 'E':
                    chosen_actions[i][j] = '1'
                elif chosen_actions[i][j] == 'S':
                    chosen_actions[i][j] = '2'
                elif chosen_actions[i][j] == 'W':
                    chosen_actions[i][j] = '3'
                output_list[i][j] = str(j + 1) + ' ' + str(i + 1) + ' ' + chosen_actions[i][j]
                

    return output_list


# Q-LEARNING
def q_learning(board=board, rewards=rewards, actions=actions, alpha=alpha, gamma=gamma, epsilon=epsilon, num_episodes=num_episodes):
    # state action pair mapping for action rewards
    Q = [[{key: 0 for key, _ in actions.iteritems()} for j in range(M)] for k in range(N)]
    # state action pair mapping for action frequencies
    #N = [[{key: 0 for key, _ in actions.iteritems()} for j in range(M)] for k in range(N)]

    rand_state = None
    rand_action = None

    regulars_list = []
    for i in range(M):
        for j in range(N):
            if board[i][j] == 'R':
                regulars_list.append((i, j))


    while num_episodes > 0: 
        # choose random state to start at each episode
        rand_state_index = random.choice(regulars_list)
        rand_state = board[rand_state_index[0]][rand_state_index[1]]

        while True:

            Q_a = Q[rand_state_index[0]][rand_state_index[1]]

            # choose an action with epsilon greedy approach
            probability = random.random()

            if probability <= epsilon:
                rand_action = random.choice(list(actions))
                rand_action_value = Q_a[rand_action] 

            else:
                # key
                rand_action = max(Q_a, key=Q_a.get)
                # value
                rand_action_value = Q_a[rand_action]


            y_coord = rand_state_index[0] + actions[rand_action][0]
            x_coord = rand_state_index[1] + actions[rand_action][1]
            outside_board = y_coord < 0 or y_coord >= M or x_coord < 0 or x_coord >= N
            if outside_board:# or board[y_coord][x_coord] == 'O':
                # stay in the current cell
                Q[rand_state_index[0]][rand_state_index[1]][rand_action] = (1 - alpha) * Q_a[rand_action] + alpha * (rewards[board[rand_state_index[0]][rand_state_index[1]]] + gamma * rand_action_value)
            else:
                Q_b = Q[y_coord][x_coord]
                next_action = max(Q_b, key=Q_b.get)
                next_action_value = Q_b[next_action]

                Q[rand_state_index[0]][rand_state_index[1]][rand_action] = (1 - alpha) * Q_a[rand_action] + alpha * (rewards[board[y_coord][x_coord]] + gamma * next_action_value)

                # next state
                rand_state = board[y_coord][x_coord]
                rand_state_index = (y_coord, x_coord)

            if rand_state == 'G':
                for key in actions.keys():
                    Q[y_coord][x_coord][key] = rewards[rand_state]
                break
            


        num_episodes = num_episodes - 1
    

    output_list = [[' ' for i in range(M)] for j in range(N)]
    chosen_actions = [[' ' for i in range(M)] for j in range(N)]

    for i in range(M):
        for j in range(N):
            output_action = max(Q[i][j], key=Q[i][j].get)
            chosen_actions[i][j] = output_action

    for i in range(M):
        for j in range(N):
            if chosen_actions[i][j] == 'N':
                chosen_actions[i][j] = '0'
            elif chosen_actions[i][j] == 'E':
                chosen_actions[i][j] = '1'
            elif chosen_actions[i][j] == 'S':
                chosen_actions[i][j] = '2'
            elif chosen_actions[i][j] == 'W':
                chosen_actions[i][j] = '3'
            output_list[i][j] = str(j + 1) + ' ' + str(i + 1) + ' ' + chosen_actions[i][j]


    return output_list


output_list = []
if alg == 'V':
    output_list = value_iteration()
elif alg == 'Q':
    output_list = q_learning()

with open(sys.argv[2], 'w') as f:
    for j in range(N):
        for i in range(M):
            print >> f, output_list[i][j]

