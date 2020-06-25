import numpy as np
from scipy.spatial.transform import Rotation as R
import matplotlib.pyplot as plt

# Use numpy arrays for matrices and vectors unless indicated otherwise!
# All angles are in radians.

class Leg:
    def __init__(self,Tfm_init, l, theta_1 = 0, theta_2 = 0, theta_3 = np.pi):
        '''
        constructor for the class.
        Tfm_init is the initial global transform of joint a1 of shape (4,4).
        l is the length of the links l1,l2.
        theta_n are the initial joint angles in the leg.
        '''
        self.l = l
        self.Tfm_a1 = Tfm_init
        self.pos_a2 = None #position vector of knee joint (a2) of shape (3,). 
        self.pos_tip = None #position vector of the tip of shape (3,).
        self.set_f_kine(theta_1, theta_2, theta_3)
        
        #update all of these fields in the setters below
        #return a bool indicating whether the update is successful.
        
    def set_Tfm_a1(self, Tfm_a1):
        '''
        changes the transformation of the base of the leg, keeps
        the joint angles constant. 
        Tfm_a1 is the new global transform of the joint a1.
        Always returns True.
        '''
        self.Tfm_a1 = Tfm_a1

        return True
        
    def set_f_kine(self, theta_1, theta_2, theta_3):
        '''
        updates the positions and transformations according to the
        new angles and the transform of the base joint. 
        Always returns True.
        '''

        transl_mat1 = np.eye(4)
        transl_mat1[0, 3] = self.l
        rot_mat1 = np.eye(4)
        rot_mat1[0:3, 0:3] = R.from_euler('y', np.pi + theta_3).as_dcm()

        rot_mat2 = np.eye(4)
        rot_mat2[0:3, 0:3] = R.from_euler('y', theta_2).as_dcm()

        rot_mat3 = np.eye(4)
        rot_mat3[0:3, 0:3] = R.from_euler('z', np.pi/2 - theta_1).as_dcm()

        global_origin = np.array([0, 0, 0, 1]).T
        # global transformation matrix for joint a2
        trans_mat_a2 = np.matmul(self.Tfm_a1, np.matmul(np.matmul( rot_mat3, rot_mat2), transl_mat1))
        self.pos_a2 = np.matmul(trans_mat_a2, global_origin)[:3,]
        # global transformation matrix for the tip
        trans_mat_tip = np.matmul(np.matmul(trans_mat_a2, rot_mat1), transl_mat1)
        self.pos_tip = np.matmul(trans_mat_tip, global_origin)[:3, ]

        return True
        
    def set_i_kine(self, pos_tip):
        '''
        updates the angles so that the tip points to pos_tip of shape (3,).
        if this is not achievable, does nothing and returns false.
        returns true otherwise.
        '''
        if self.is_reachable(pos_tip):
            self.set_f_kine(*self.i_kine(pos_tip))
            return True
        else:
            return False
    
    def is_reachable(self, pos_tip):
        '''
        returns True if pos_tip is reachable by the leg tip. returns
        False otherwise.
        See set_i_kine.
        '''
        pos_tip = np.array([pos_tip[0,], pos_tip[1,], pos_tip[2,]]).T

        global_origin = np.array([0, 0, 0, 1]).T
        # coordinates of joint a1 in the global frame
        pos_a1 = np.matmul(self.Tfm_a1, global_origin)[:3, ]
        
        theta_1, theta_2, theta_3 = self.i_kine(pos_tip)
        # angle constraints given in the homework text
        if theta_1 > -np.pi/2 and theta_1 < np.pi/2:
            if theta_2 > -np.pi/2 and theta_2 < np.pi/2:
                if theta_3 > -np.pi and theta_3 < 0:
                    # distance from the tip to the base should be smaller than the sum of link lengths
                    if np.linalg.norm(pos_a1 - pos_tip) <= 2*self.l:
                        return True
        return False
        
    
    # operations are from the solution of the theoretical part
    def i_kine(self, pos_tip):
        '''
        calculates inverse kinematics for the leg.
        See set_i_kine.
        '''
        pos_tip_4d = np.array([pos_tip[0,], pos_tip[1,], pos_tip[2,], 1]).T
        # coordinates of the tip w.r.to the leg base.
        pos_tip_leg_space = np.matmul(np.linalg.inv(self.Tfm_a1), pos_tip_4d)[:3,]

        # theta_1
        theta_1 = np.arctan(pos_tip_leg_space[0,]/pos_tip_leg_space[1,])

        # d: distance from tip to base
        d = np.linalg.norm(pos_tip_leg_space)
        # theta_3
        theta_3 = -np.arccos((2 * self.l * self.l - d*d)/(2 * self.l * self.l))
        beta = np.arcsin(self.l * np.sin((theta_3 + np.pi))/d)
        # theta_2
        theta_2 = np.arcsin(pos_tip_leg_space[2,]/d) - beta

        return (theta_1,theta_2,theta_3)


class Sphinx:

    def __init__(self, Tfm_init,d1,d2,l):
        '''
        constructor for the sphinx.
        Tfm_init is the initial global transform of the body.
        d1,d2 are the parameters of the body.
        l is the link lengths for the legs.
        
        Initialize with tip of the legs fixed at global z = 0 and
        their global x,y are the same as their base joint (the a1 joint)
        Tfm_init will be given such that this is guaranteed to be
        achievable.
        '''
        
        self.Tfm = Tfm_init
        self.d1 = d1
        self.d2 = d2
        self.l = l

        rot_p3_to_s = np.eye(4)
        rot_p3_to_s[0:3, 0:3] = R.from_euler('x', - np.pi/2).as_dcm()
        transl_p3_to_s = np.eye(4)
        transl_p3_to_s[0, 3] = - self.d1

        # transformation from p3 to world
        Tfm_p3_to_s = np.matmul(transl_p3_to_s, rot_p3_to_s)
        Tfm_p3 = np.matmul(Tfm_init, Tfm_p3_to_s)

        # transformation from p2 to world
        transl_p3_to_s[0, 3] = self.d1
        Tfm_p2_to_s = np.matmul(transl_p3_to_s, rot_p3_to_s)
        Tfm_p2 = np.matmul(Tfm_init, Tfm_p2_to_s)

        # transformation from p1 to world
        transl_p1_to_p2 = np.eye(4)
        transl_p1_to_p2[2, 3] = - self.d2
        Tfm_p1_to_s = np.matmul(Tfm_p2_to_s, transl_p1_to_p2)
        Tfm_p1 = np.matmul(Tfm_init, Tfm_p1_to_s)

        
        #self.p1 = None # The fields for the legs. All of them are Leg objects. Initialize them accordingly.
        global_origin = np.array([0, 0, 0, 1]).T
        # p1 
        self.p1 = Leg(Tfm_p1, l)
        pos_p1_a1 = np.matmul(Tfm_p1, global_origin)[:3,]
        pos_p1_tip = np.array([pos_p1_a1[0,], pos_p1_a1[1,], 0]).T
        self.p1.set_i_kine(pos_p1_tip)

        # p2
        self.p2 = Leg(Tfm_p2, l)
        pos_p2_a1 = np.matmul(Tfm_p2, global_origin)[:3,]
        pos_p2_tip = np.array([pos_p2_a1[0,], pos_p2_a1[1,], 0]).T
        self.p2.set_i_kine(pos_p2_tip)

        # p3
        self.p3 = Leg(Tfm_p3, l)
        pos_p3_a1 = np.matmul(Tfm_p3, global_origin)[:3,]
        pos_p3_tip = np.array([pos_p3_a1[0,], pos_p3_a1[1,], 0]).T
        self.p3.set_i_kine(pos_p3_tip)
    

    # same operations as the constructor, except the accessibility check and keeping leg tip coordinates unchanged.
    def set_Tfm_fixed_legs(self, Tfm):
        '''
        Tfm is the proposed transform of the body with shape (4,4).
        
        updates the positions and transformations of the legs and body
        according to the new global Tfm, while keeping leg tips fixed on the
        ground, without violating joint constraints. If this is achievable,
        returns True and updates the relevant fields. Otherwise, keeps
        the relevant fields as is and returns False.
        '''

        # save leg tip coordinates
        p1_pos_tip = np.array(self.p1.pos_tip)
        p2_pos_tip = np.array(self.p2.pos_tip)
        p3_pos_tip = np.array(self.p3.pos_tip)

        self.Tfm = Tfm

        rot_p3_to_s = np.eye(4)
        rot_p3_to_s[0:3, 0:3] = R.from_euler('x', - np.pi/2).as_dcm()
        transl_p3_to_s = np.eye(4)
        transl_p3_to_s[0, 3] = - self.d1

        # transformation from p3 to world
        Tfm_p3_to_s = np.matmul(transl_p3_to_s, rot_p3_to_s)
        Tfm_p3 = np.matmul(Tfm, Tfm_p3_to_s)

        # transformation from p2 to world
        transl_p3_to_s[0, 3] = self.d1
        Tfm_p2_to_s = np.matmul(transl_p3_to_s, rot_p3_to_s)
        Tfm_p2 = np.matmul(Tfm, Tfm_p2_to_s)

        # transformation from p1 to world
        transl_p1_to_p2 = np.eye(4)
        transl_p1_to_p2[2, 3] = - self.d2
        Tfm_p1_to_s = np.matmul(Tfm_p2_to_s, transl_p1_to_p2)
        Tfm_p1 = np.matmul(Tfm, Tfm_p1_to_s)

        
        # transform joints for each leg
        # 1st leg
        global_origin = np.array([0, 0, 0, 1]).T
        self.p1 = Leg(Tfm_p1, self.l)
        # set a1
        pos_p1_a1 = np.matmul(Tfm_p1, global_origin)[:3,]
        # flag to check if the tf is achievable
        p1_true = self.p1.set_i_kine(p1_pos_tip)

        # same for the other two legs
        self.p2 = Leg(Tfm_p2, self.l)
        pos_p2_a1 = np.matmul(Tfm_p2, global_origin)[:3,]
        p2_true = self.p2.set_i_kine(p2_pos_tip)

        self.p3 = Leg(Tfm_p3, self.l)
        pos_p3_a1 = np.matmul(Tfm_p3, global_origin)[:3,]
        p3_true = self.p3.set_i_kine(p3_pos_tip)

        # check if transformation is achievable
        if p1_true and p2_true and p3_true:
            #self.Tfm = Tfm
            # set a1
            #self.p1 = Leg(Tfm_p1, self.l)
            #self.p2 = Leg(Tfm_p2, self.l)
            #self.p3 = Leg(Tfm_p3, self.l)
            # set a2
            self.p1.set_i_kine(p1_pos_tip)
            self.p2.set_i_kine(p2_pos_tip)
            self.p3.set_i_kine(p3_pos_tip)
            return True

        return False
        
def quaternion_slerp(quat_1,quat_2,alpha):
    '''
    quot_1,quot_2 are quoternions w<a,b,c> represented as numpy arrays [w,a,b,c] with shape (4,)
    alpha is the interpolation value, an np.double taking values between 0 and 1.
    
    returns the quoternion interpolation of quat_1 and quat_2
    if alpha == 0, this function returns quat_1,
    if alpha == 1, this function returns quat_2.
    '''
    # used the formula slerp(q1, q2, alpha) = q1 * (q1^{-1} * q2)^alpha
    # * : quaternion multiplication
    # q^{-1} : conjugate of q

    # q1^{-1}
    q1_conj = np.array([quat_1[0,], -quat_1[1,], -quat_1[2,], -quat_1[3,]])

    # q3 = q1^{-1} * q2
    q1_conj_q2 = np.ndarray(shape=(4,), dtype='double')
    q1_conj_q2[0,] = q1_conj[0,] + quat_2[0,] - np.dot(q1_conj[1:,], quat_2[1:,])
    q1_conj_q2[1:,] = q1_conj[0,] * quat_2[1:,] + quat_2[0,] * q1_conj[1:,] + np.cross(q1_conj[1:,], quat_2[1:,])

    # q3 as rotvec
    q1_conj_q2_rotvec = R.from_quat(q1_conj_q2).as_rotvec()

    # angle of rotation for q3
    q1_conj_q2_angle = np.linalg.norm(q1_conj_q2_rotvec)

    # for the exponential, i used the formula:
    # q = cos(theta) + v * sin(theta)
    # q^alpha = cos(alpha*theta) + v*sin(alpha*theta)

    # alpha*q3_angle
    new_angle = q1_conj_q2_angle * alpha
    # q3^alpha
    new_q = np.ndarray(shape=(4,))
    new_q[0,] = np.cos(new_angle/2)
    new_q[1:,] = np.sin(new_angle/2) * q1_conj_q2_rotvec
    # slerp: resulting quaternion
    slerp = np.ndarray(shape=(4,), dtype='double')
    slerp[0,] = quat_1[0,] + new_q[0,] - np.dot(quat_1[1:,], new_q[1:,])
    slerp[1:,] = quat_1[0,] * new_q[1:,] + new_q[0,] * quat_1[1:,] + np.cross(quat_1[1:,], new_q[1:,])

    return slerp
    #pass

def position_lerp(pos_1,pos_2,alpha):
    '''
    returns the linear interpolation between pos_1,pos_2 of size (3,).
    alpha is the interpolation value, an np.double taking values between 0 and 1.
    if alpha == 0, this function returns pos_1,
    if alpha == 1, this function returns pos_2.
    '''
    new_pos = alpha * pos_2 + (1 - alpha) * pos_1
    return new_pos
    #pass

def interpolate_Tfms(Tfm_1,Tfm_2,alpha):
    '''
    returns the simultaneous positional and orientational interpolation between Tfm_1,Tfm_2 of size (4,4).
    alpha is the interpolation value, an np.double taking values between 0 and 1.
    if alpha == 0, this function returns Tfm_1,
    if alpha == 1, this function returns Tfm_2.
    '''
    q1 = R.from_dcm(Tfm_1[:3, :3]).as_quat()
    q2 = R.from_dcm(Tfm_2[:3, :3]).as_quat()

    slerp = quaternion_slerp(q1, q2, alpha)

    rot_matrix = R.from_quat(slerp).as_dcm()

    pos_1 = Tfm_1[:, 3]
    pos_2 = Tfm_2[:, 3]

    lerp = position_lerp(pos_1, pos_2, alpha)

    new_tfm = np.ndarray(shape=(4, 4), dtype='double')
    new_tfm[:3, :3] = rot_matrix
    new_tfm[:, 3] = lerp

    return new_tfm
    #pass
    



	
