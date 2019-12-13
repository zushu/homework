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

#def replace(clause, value):
#    for elem in clause:
#        for param in elem[1]:



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
                    #    for param1 in elem1[1]:
                    #        for param2 in elem2[1]:
                    #            if param1[1] == [] and param2[1] == []:
                    #                if param2[0].isupper():








def theorem_prover(premises_list, negated_goal):
    premises_parsed = [parse_clause(item) for item in premises_list]
    #premises_parsed.append(parse_clause(negated_goal))
    premises_parsed = after_tautology_elimination(premises_parsed)
    premises_parsed = after_subsumption(premises_parsed)

    goal_parsed = parse_clause(negated_goal)







    return ('no', [])


