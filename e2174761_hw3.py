# takes a constant (function constant or atomic constant)
# returns a two-tuple of function name and parameters
def extract_parameters(literal):
    if len(literal.split('(')) == 1:
        # not a good solution but works for now
        return (literal.rsplit(')', 1)[0], [])

    else:
        func_name = literal.split('(', 1)[0].rsplit(')', 1)[0]
        params = literal.split('(', 1)[1].rsplit(')', 1)[0].split(',') 

        return (func_name, [extract_parameters(param) for param in params])

def parse_clause(clause):
    literals = clause.split('+')
    result = []
    for literal in literals:
         result.append(extract_parameters(literal))

    return result

def reverse_parsing_item(parsed_item):
    result = ''
    if parsed_item[1] == []:
        return parsed_item[0]
    else:
        result = parsed_item[0]
        for i in range(len(parsed_item[1])):
            result = result + '(' + reverse_parsing_item(parsed_item[1][i])
            if i != len(parsed_item[1]) - 1:
                result = result + ','
        result = result + ')'
    
    return result

def reverse_parsing_clause(parsed_clause):
    result = ''
    for i in range(len(parsed_clause)):
        result = result + reverse_parsing_item(parsed_clause[i])
        if i != len( parsed_clause) - 1:
            result = result + '+'
    return result


def replace(clause_item, param, value):
    #for elem in clause:
    #    for param in elem[1]:
    if clause_item[1] == []:
        if clause_item == param:
            return value
        else:
            return clause_item
    #elif clause_item[0] != param and clause_item[1] == []:
     #   return clause_item
    else:
        return (clause_item[0], [replace(clause_item_member, param, value) for clause_item_member in clause_item[1]])
        
def replace_in_clause(clause, param, value):
    new_clause = []
    for elem in clause:
        new_clause.append(replace(elem, param, value))

    return new_clause

# resolve disjuncts
# find MGU and return it
def find_replacement_value(clause_item1, clause_item2):
    if clause_item1[1] == [] and clause_item2[1] == []:
        if clause_item1[0].isupper():
            return clause_item1
        elif clause_item2[0].isupper():
            return clause_item2
        elif clause_item1[0].islower() and clause_item2[0].islower():
            return clause_item1

    elif clause_item1[1] == [] and clause_item2[1] != []:
        return clause_item2
    elif clause_item1[1] != [] and clause_item2[1] == []:
        return clause_item1    
    else:
        case1 = clause_item1[0] == clause_item2[0]
        #case2 =  clause_item1[0].startswith('~') and clause_item1[0][1:] == clause_item2[0]
        #case3 =  clause_item2[0].startswith('~') and clause_item2[0][1:] == clause_item1[0]
        
        if case1: #or case2 or case3:
            return (clause_item1[0], 
                    [find_replacement_value(clause_item1_member1, clause_item2_member2) 
                    for clause_item1_member1, clause_item2_member2 
                    in zip(clause_item1[1], clause_item2[1])])

# returns (param, value)
def find_replacement_value2(clause_item1, clause_item2):
    result = []
    if clause_item1[1] == [] and clause_item2[1] == []:
        if clause_item1[0].isupper():
            return [(clause_item2, clause_item1)]
        elif clause_item2[0].isupper():
            return [(clause_item1, clause_item2)]
        elif clause_item1[0].islower() and clause_item2[0].islower():
            return [(clause_item2, clause_item1)]

    elif clause_item1[1] == [] and clause_item2[1] != []:
        return [(clause_item1, clause_item2)]
    elif clause_item1[1] != [] and clause_item2[1] == []:
        return [(clause_item2, clause_item1)]
    else:
        case1 = clause_item1[0] == clause_item2[0]
        #case2 =  clause_item1[0].startswith('~') and clause_item1[0][1:] == clause_item2[0]
        #case3 =  clause_item2[0].startswith('~') and clause_item2[0][1:] == clause_item1[0]
        
        if case1: #or case2 or case3:
            #return [find_replacement_value2(clause_item1_member1, clause_item2_member2) 
             #       for clause_item1_member1, clause_item2_member2 
            #        in zip(clause_item1[1], clause_item2[1])]
            for clause_item1_member1, clause_item2_member2 in zip(clause_item1[1], clause_item2[1]):
                result.append(find_replacement_value2(clause_item1_member1, clause_item2_member2)[0])
    return result
            
# resolve clauses
# find MGU and return it
def resolution(clause1, clause2):
    new_clause = []
    #new_clause = clause1.extend(clause2)
    clause1_tmp = clause1 
    clause2_tmp = clause2
    found = False
    for clause_item1 in clause1_tmp:
        for clause_item2 in clause2_tmp:
            if clause_item1[0].startswith('~'):
                if clause_item2[0] == clause_item1[0][1:]:
                    new_item = find_replacement_value((clause_item1[0][1:], clause_item1[1]), clause_item2)
                    if new_item != None:
                        found = True
                        params_values_to_be_replaced = find_replacement_value2((clause_item1[0][1:], clause_item1[1]), clause_item2)
                        # remove old

                        clause1_tmp.remove(clause_item1)
                        clause2_tmp.remove(clause_item2)                        
                        temp = clause1_tmp
                        temp.extend(clause2_tmp)
                        new_clause = temp
                        # make replacements
                        if params_values_to_be_replaced != None:
                            for (param, value) in params_values_to_be_replaced:
                                new_clause = replace_in_clause(new_clause, param, value)
                        return new_clause

            elif clause_item2[0].startswith('~'):
                if clause_item1[0] == clause_item2[0][1:]:
                    new_item = find_replacement_value((clause_item2[0][1:], clause_item2[1]), clause_item1)
                    if new_item != None:
                        found = True
                        params_values_to_be_replaced = find_replacement_value2((clause_item2[0][1:], clause_item2[1]), clause_item1)
                        # remove old
                        clause1_tmp.remove(clause_item1)
                        clause2_tmp.remove(clause_item2) 

                        temp = clause1_tmp
                        temp.extend(clause2_tmp)
                        new_clause = temp
                        # make replacements
                        if params_values_to_be_replaced != None:
                            for param, value in params_values_to_be_replaced:
                                new_clause = replace_in_clause(new_clause, param, value)
                        return new_clause

    return found

def is_tautology(clause):  
    negated = ('',[]) 
    for disjunct in clause:
        if disjunct[0].startswith('~'):
            negated = (disjunct[0][1:], disjunct[1])  

    for disjunct in clause:
        if disjunct == negated:
            return True

    return False

def after_tautology_elimination(clauses):
    for clause in clauses:
        if is_tautology(clause):
            clauses.remove(clause)
    return clauses

def subsumes(clause1, clause2):
    func_names_1 = [disjunct[0] for disjunct in clause1]
    func_names_2 = [disjunct[0] for disjunct in clause2]

    for name1 in func_names_1:
        for name2 in func_names_2:
            if name1 == name2:
                func_names_1.remove(name1)
                func_names_2.remove(name2)
                break
    
    if func_names_1 == []:
        return True

    return False
    #return all(elem in func_names_2 for elem in func_names_1)


def after_subsumption(clauses):
    #parsed_clauses = [parse_clause(clause) for clause in clauses]
    for clause1 in clauses:
        for clause2 in clauses:
            if clause2 != clause1:
                if subsumes(clause1, clause2):
                    clauses.remove(clause2)
                    break

    return clauses


def theorem_prover(premises_list, negated_goal):
    res_list = []
    return_list = []
    premises_parsed = [parse_clause(item) for item in premises_list]
    premises_parsed = after_tautology_elimination(premises_parsed)
    premises_parsed = after_subsumption(premises_parsed)

    goal_parsed = [parse_clause(item) for item in negated_goal]

    set_of_support = goal_parsed

    while set_of_support != []:
        clause1 = set_of_support.pop()
        for clause2 in premises_parsed:
            reversed_clause1 = reverse_parsing_clause(clause1)
            reversed_clause2 = reverse_parsing_clause(clause2)
            res = resolution(clause1, clause2)
            if res != False:
                if res != []:
                    set_of_support = [res] + set_of_support
                    return_val = reversed_clause1 + '$' + reversed_clause2 + '$' + reverse_parsing_clause(res)
                    return_list.append(return_val)
                else:
                    return_val = reversed_clause1 + '$' + reversed_clause2 + '$' + 'empty'
                    return_list.append(return_val)
                    return ('yes', return_list)

    #print return_list
    return ('no', [])

#print theorem_prover(["p(A,f(t))", "q(z)+~p(z,f(B))", "~q(y)+r(y)", "m(C)+~m(x)"],["~r(A)"])
#print theorem_prover(["p(A,f(t))", "q(z)+~p(z,f(B))", "q(y)+r(y)", "~q(x)+m(x)"],["~r(A)"])
