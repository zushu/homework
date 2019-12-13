def eliminate_tautologies(clause):
    
    return


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


