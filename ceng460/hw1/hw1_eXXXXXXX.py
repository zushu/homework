import numpy as np
from scipy.spatial.transform import Rotation as R
import matplotlib.pyplot as plt

def simulate(o_init, a_init, inputs, d):
    """
    Simulates the arms.
    
    Parameters:
        o_init (np.ndarray): A 4 x 4 transformation matrix describing 
        the state of o relative to global frame.
        
        a_init (np.ndarray): A 4 x 4 x n_r matrix. Each a_init[:, :, i] 
        is a 4 x 4 transformation of the free end of rod_1 relative to
        global frame. (M1 in the Figure 1 of assignment text.)
        
        inputs(np.ndarray): A 4 x m x n_r matrix. Each inputs[:,:,i]
        is a 4 x m matrix describing l1,l2,theta and h of arm i, 
        during time steps 0..m-1. inputs[:,j,i] corresponds to a column
        vector [l1[j],l2[j],theta[j],h[j]] for time step j for arm i.
        theta is in radians.
        
        d (np.double): The radius of the sphere at the free end of rod_2.
        
    Returns:
        o_all (np.nd.array): A 4 x 4 x m matrix. Each o_all[:,:,i] is a
        4 x 4 transform of the o relative to global frame at time step i
        
        M12_all (np.nd_array): A 4 x 4 x m x n_r matrix. Each M12_all[:,:,i,j]
        is a 4 x 4 transform of M12 relative to global frame (defined
        in the assignment text) at time step i for arm j
        
        M2_all (np.nd_array): A 4 x 4 x m x n_r matrix. Each M2_all[:,:,i,j]
        is a 4 x 4 transform of M2 relative to global frame (defined
        in the assignment text) at time step i for arm j
        
        parent_all (np.nd_array): A 1xm array indicating which arm is
        grabbing o. parent_all[i] = j if arm j is grabbing the object at
        time step i. If o is not being grabbed by any arm at i,
        parent_all[i] = -1.
        
    """

    n_r = a_init.shape[2]
    m = inputs.shape[1]
    o_all = np.ndarray(shape=(4, 4, m), dtype='float')
    M12_all = np.ndarray(shape=(4, 4, m, n_r), dtype='float')
    M2_all = np.ndarray(shape=(4, 4, m, n_r), dtype='float')
    parent_all = np.ndarray(shape=(m), dtype='int')


    # initializing transformations separately for t = 0 case
    # TODO: Check it again, might be unnecessary
    t = 0
    o_all[:, :, t] = o_init
    parent_all[0] = -1
    for arm_i in range(n_r):
        # info: vector [l1, l2, theta, h]
        info = inputs[:, t, arm_i]
        M2_all[:, :, t, arm_i] = np.matmul(a_init[:, :, arm_i], T_3(info[0], info[1], info[2]))
        M12_all[:, :, t, arm_i] = np.matmul(a_init[:, :, arm_i], T_1(info[0]))
        distance_from_object = np.linalg.norm(M2_all[:, 3, t, arm_i]-o_all[:, 3, t])
        c_sph = distance_from_object < d       

    # t - time instance
    for t in range(1, m):
        # arm_i - arm id
        for arm_i in range(n_r):
            # info: vector [l1, l2, theta, h]
            info = inputs[:, t, arm_i].T
            M2_all[:, :, t, arm_i] = np.matmul(a_init[:, :, arm_i], T_3(info[0], info[1], info[2]))
            M12_all[:, :, t, arm_i] = np.matmul(a_init[:, :, arm_i], T_1(info[0]))
            distance_from_object = np.linalg.norm(M2_all[:, 3, t-1, arm_i]-o_all[:, 3, t-1])
            c_sph = distance_from_object < d

            # h_a_j
            if info[3] == 0:
                if parent_all[t-1] == arm_i:
                    for arm_j in range(n_r):
                        info_j = inputs[:, t, arm_j].T
                        M2_all[:, :, t, arm_j] = np.matmul(a_init[:, :, arm_j], T_3(info_j[0], info_j[1], info_j[2]))
                        M12_all[:, :, t, arm_j] = np.matmul(a_init[:, :, arm_j], T_1(info_j[0]))
                        distance_from_object = np.linalg.norm(M2_all[:, 3, t-1, arm_j]-o_all[:, 3, t-1])
                        c_sph = distance_from_object < d
                        if info_j[3] == 1:
                            if c_sph:
                                parent_all[t] = arm_j
                                T_o_to_M2 = np.matmul(np.linalg.inv(M2_all[:, :, t-1, arm_j]), o_all[:, :, t-1])    
                                o_all[:, :, t] = np.matmul(M2_all[:, :, t, arm_j], T_o_to_M2)
                        else:
                            parent_all[t] = -1

            elif info[3] == 1:
                if c_sph:
                    if parent_all[t-1] == -1 or parent_all[t-1] == arm_i or parent_all[t] == -1:
                        parent_all[t] = arm_i
                        # move the object
                        # transformation from o to M2
                        T_o_to_M2 = np.matmul(np.linalg.inv(M2_all[:, :, t-1, arm_i]), o_all[:, :, t-1])        
                        o_all[:, :, t] = np.matmul(M2_all[:, :, t, arm_i], T_o_to_M2)
   
    return (o_all, M12_all, M2_all, parent_all)


# directly taken from the written part solution, no calculation steps performed
def T_1(l1):
    """
    Returns:
    tfm_mat (np.ndarray): T_1 in the assignment, Theory-a part.
    """
    tfm_mat = np.eye(4)
    tfm_mat[0, 3] = l1   
    return tfm_mat

def T_2(l2,theta):
    """
    Returns:
    tfm_mat (np.ndarray): T_2 in the assignment, Theory-a part.
    theta is in radians.
    """

    theta2 = -(np.pi - theta)
    tfm_mat = np.eye(4)
    tfm_mat[0:3, 0:3] = R.from_euler('x', theta2).as_dcm()
    tfm_mat[1, 3] = l2 * np.cos(theta)
    tfm_mat[2, 3] = l2 * np.sin(theta)  
    return tfm_mat

def T_3(l1,l2,theta):
    """
    Returns:
    tfm_mat (np.ndarray): T_3 in the assignment, Theory-a part.
    theta is in radians.
    """

    tfm_mat = np.matmul(T_1(l1), T_2(l2, theta))   
    return tfm_mat


