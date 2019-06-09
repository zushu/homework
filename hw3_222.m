% written in MATLAB R2018a

% 1 - alpha = 0.99
alpha = 0.01;

% from table A4 phi^{-1} (1 - alpha/2) = phi^{-1} (0.995) = 2.5
z = 2.5;

epsilon = 0.02;

% size of the monte carlo study
N = ceil(0.25 * (z / epsilon)^2);

%lambdas for number of vehicles (poisson)
l_num_motor = 40;
l_num_auto = 30;  
l_num_truck = 20; 

%lambdas for weight of each vehicle (gamma)
l_wght_motor = 0.15;
l_wght_auto = 0.05;  
l_wght_truck = 0.01; 

%alphas for weight of each vehicle (gamma)
a_wght_motor = 16; 
a_wght_auto = 60;  
a_wght_truck = 84; 

% poisson distribution vectors
P_motor = zeros(N, 1);
P_auto = zeros(N, 1); 
P_truck = zeros(N, 1); 

total_weight_motor = zeros(N, 1);
total_weight_auto = zeros(N, 1);
total_weight_truck = zeros(N, 1);

for k = 1:N
    P_motor(k) = poissondist(l_num_motor);
    P_auto(k) = poissondist(l_num_auto);
    P_truck(k) = poissondist(l_num_truck);
       
    total_weight_motor(k) = totalweight(P_motor(k), l_wght_motor, a_wght_motor);
    total_weight_auto(k) = totalweight(P_auto(k), l_wght_auto, a_wght_auto);
    total_weight_truck(k) = totalweight(P_truck(k), l_wght_truck, a_wght_truck);   
end

% Nx1 vector
total_weight = total_weight_motor + total_weight_auto + total_weight_truck;
% P{total_weight > 220 tons}
P = mean(total_weight > 220000);

% expectance
X = mean(total_weight);
% standard deviation
S = std(total_weight);

% poisson distribution function
function output = poissondist(lambda)
    U = rand;
    i = 0;
    F = exp(-lambda);
    while (U >= F)
        i = i+1;
        F = F + exp(-lambda) * lambda ^ i / gamma(i + 1);
    end
    output = i;
end

% gamma distribution function
function output = gammadist(lambda, alpha)
    output = sum(-1/lambda * log(rand(alpha, 1)));
end

% sum of weights (weight is a Gamma distributed variable with 
% parameters `lambda` and `alpha`) of `num_vehicles` amount of vehicles 
function output = totalweight(num_vehicles, lambda, alpha)
    single_weights = zeros(num_vehicles, 1);
    for j = 1:num_vehicles
        single_weights(j) = gammadist(lambda, alpha);
    end
    
    output = sum(single_weights);
end


