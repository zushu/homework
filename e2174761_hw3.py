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

     
#def replace(literal, replacement_value):
#    replaced_literal = (literal[0], replacement_value)
#    return replaced_literal

def replace(clause_item, param, value):
    #for elem in clause:
    #    for param in elem[1]:
    if clause_item[1] == []:
        if clause_item[0] == param:
            return (value, [])
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

"""
def find_replacement_value(clause_item1, clause_item2):
    if clause_item1[1] == [] and clause_item2[1] == []:
        # both constant
        if clause_item1[0].isupper() and clause_item2[0].isupper():
            return clause_item1[0]
        # 1st const, 2nd variable
        elif clause_item1[0].isupper() and clause_item2[0].islower():
            return clause_item1[0]
        # 1st var, 2nd const
        elif clause_item1[0].islower() and clause_item2[0].isupper():
            return clause_item2[0]
        # both var
        else:
            return clause_item2[0]
    
    else:
"""



def is_tautology(clause):
    #disjuncts = parse_clause(clause)  
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
            if subsumes(clause1, clause2):
                clauses.remove(clause2)
                break

    return clauses


def resolution(clause1, clause2):
    res_exists = False
    for elem1 in clause1:
        for elem2 in clause2:
            if elem1[0].startswith('~'):
                if elem1[0][1:] == elem2[0]:
                    if elem1[1] == elem2[1]:
                        res_exists = True
                        return list(set(clause1.remove(elem1)) | set(clause2.remove(elem2))) 
                    #else:
                        # TODO: find replacement value, call replace_in_clause function
                        #for param1 in elem1[1]:
                        #    for param2 in elem2[1]:
                        #        if param1[1] == [] and param2[1] == []:
                        #            if param2[0].isupper():


    return res_exists








def theorem_prover(premises_list, negated_goal):
    premises_parsed = [parse_clause(item) for item in premises_list]
    #premises_parsed.append(parse_clause(negated_goal))
    premises_parsed = after_tautology_elimination(premises_parsed)
    premises_parsed = after_subsumption(premises_parsed)

    goal_parsed = parse_clause(negated_goal)

    set_of_support = [goal_parsed]

    while set_of_support != []:
        clause1 = set_of_support.pop()
        for clause2 in premises_parsed:
            resolution(clause1, clause2)





    return ('no', [])


