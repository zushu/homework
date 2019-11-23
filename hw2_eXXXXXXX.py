import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import solve_ivp

def simulate_moving_to_a_point(q_init, goal, K_pv, K_pth, t_eval, d, options):
    
    """
    Simulates a differential drive moving toward a point with P control.
    
    Parameters:
        q_init(np.ndarray): initial state [x y theta] of the robot.
        goal(np.ndarray): goal point [x y].
        K_pv(np.double): velocity gain.
        K_pth(np.double): steering gain.
        t_eval(np.ndarray): the time instances to be solved by solver. has shape (n,).
            d(np.double): the simulation should terminate if the distance of
            the robot to the goal is less than this value.
        options(dict): options for the solver. Directly pass it as keyword args.
    Returns:
        t(np.ndarray): solved time instances. This should be the same as t_eval
        unless the simulation is terminated by an event function. has shape (m,).
        q(np.ndarray):robot pose solutions for each element of t. has shape (3,m).
        bunch(scipy.integrate._ivp.ivp.OdeResult): bunch object returned by the solver.
    
    """
    
    def diffeqn(t,q):
        vel_star = K_pv * np.linalg.norm(q[0:2] - goal) # np.sqrt((goal[0] - q[0])**2 + (goal[1] - q[1])**2)
        theta_star = K_pth * np.arctan2((q[1] - goal[1]), (q[0] - goal[0])) #np.arctan2((goal[1] - q[1]), (goal[0] - q[0]))
        dq = [vel_star * np.cos(theta_star), vel_star * np.sin(theta_star), theta_star]
        return dq
    
    def term_event(t,q):
        return np.linalg.norm(q[0:2]-goal) - d
    
    term_event.terminal = True
    term_event.direction = -1
    
    bunch = solve_ivp(diffeqn, t_eval[[0,-1]], q_init, t_eval=t_eval, events=[term_event],**options)
    t = bunch.t
    q = bunch.y[0:3,:]

    #print(t)
    
    return (t,q,bunch)
    
def simulate_moving_with_a_trajectory(q_init, goal_pts, K_pv, K_pth, t_eval, d, options):
    """
    Simulates a differential drive moving with a trajectory with P control.
    
    Parameters:
        q_init(np.ndarray): initial state [x y theta] of the robot.
        goal_pts(np.ndarray): goal points with rows [x y]. has shape (l,2).
        K_pv(np.double): velocity gain.
        K_pth(np.double): steering gain.
        t_eval(np.ndarray): the time instances to be solved by solver. has shape (n,).
        d(np.double): the partial simulation should terminate for the current 
            goal point if the distance of the robot to the goal is less than this value.
            Then it should restart with the next point as the goal point in goal_pts. 
            If this was the last point, the simulation should truly terminate.
        options(dict): options for the solver. Directly pass it as keyword args.
    Returns:
        t(np.ndarray): solved time instances. This should be the same as t_eval
        unless the robot reaches the last point earlier than t_eval[-1]. has shape (m,).
        q(np.ndarray):robot pose solutions for each element of t. has shape (3,m).
        inds(np.ndarray): the index of the current goal in goal_pts at that time instance.
            has shape (m,).
        bunches(list): All bunch objects returned along with the partial solutions.
    
    """    

    t = np.ndarray(shape=(0,), dtype='float')
    q = np.ndarray(shape=(3, 0), dtype='float')
    inds = np.ndarray(shape = (0,), dtype='int')
    #t = []
    #q = []
    #inds = []
    bunches = []
    t_eval_copy = t_eval

    num_goal_pts = goal_pts.shape[0]
    for i in range(num_goal_pts):
        if i == 0:
            (t_0, q_0, bunch_0) = simulate_moving_to_a_point(q_init, goal_pts[i, :], K_pv, K_pth, t_eval, d, options)
            #t_eval_copy = np.setdiff1d(t_eval_copy,t_0)
            t_eval_copy = np.delete(t_eval_copy, np.where(t_eval_copy == t_0))
            inds = np.append(inds, 0)
            t = np.append(t, t_0)
            q = np.append(q, q_0, axis=1)
            bunches.append(bunch_0)

        else:
            (t_i, q_i, bunch_i) = simulate_moving_to_a_point(bunches[i - 1].y[0:3, -1], goal_pts[i, :], K_pv, K_pth, t_eval_copy, d, options)
            #t_eval_copy = np.setdiff1d(t_eval_copy,t_i)
            t_eval_copy = np.delete(t_eval_copy, np.where(t_eval_copy == t_i))
            t = np.append(t, t_i)
            q = np.append(q, q_i, axis=1)
            inds = np.append(inds, i)
            bunches.append(bunch_i)
        
    return (t, q, inds, bunches)
    
    
