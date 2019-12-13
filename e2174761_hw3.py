def is_tautology(clause):
    disjuncts = parse_clause(clause)  
    negated = ('',[]) 
    for disjunct in disjuncts:
        if disjunct[0].startswith('~'):
            negated = (disjunct[0][1:], disjunct[1])  

    for disjunct in disjuncts:
        if disjunct == negated:
            return True

    return False

def after_subsumption(clauses):
    parsed_clauses = [parse_clause(clause) for clause in clauses]
 
def replace(literal, replacement_value):
    replaced_literal = (literal[0], replacement_value)
    return replaced_literal
    

def parse_clause(clause):
    literals = clause.split('+')
    result = []
    for literal in literals:
         result.append(extract_parameters(literal))

    return result
    
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



def theorem_prover(premises_list, negated_goal):
    return ('no', [])


