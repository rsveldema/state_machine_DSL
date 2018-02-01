from stringify import *

""" Creates a control-flow-graph of a code fragment
"""

node_counter = 0;

def add_succ(prev_list, s):
    for n in prev_list:
        n.add_next(s);
    del prev_list[:]
    prev_list.append(s)


def clone_list(lst):
    ret = []
    for i in lst:
        ret.append(i)
    return ret
            

class Node:
    def __init__(self, stmt, name, prev_list):
        global node_counter;
        self.stmt = stmt
        self.name = name
        self.label = "n_" + str(node_counter)
        node_counter += 1
        self.next = []
        add_succ(prev_list, self)

    def add_next(self, n):
        if not n in self.next:
            self.next.append(n)
        
    def dot(self, f, seen):
        if self in seen:
            return
        seen[self] = self
        f.write(self.label + " [label=\""+self.name+"\"];\n");
        for a in self.next:
            f.write(self.label + " -> " + a.label + ";\n");
        for n in self.next:
            n.dot(f, seen)


def add(ret, t):
    for n in t:
        if not n in ret:
            ret.append(n)
            
def cfg_if(s, prev_list):
    start = Node(s, "if", prev_list)
    stats = s.statement()
    if len(stats) == 1:
        prev_if   = clone_list(prev_list)
        cfg_stmt(stats[0], prev_if)
        add(prev_list, prev_if)
    else:
        prev_if1   = clone_list(prev_list)
        prev_if2   = clone_list(prev_list)

        cfg_stmt(stats[0], prev_if1)
        cfg_stmt(stats[1], prev_if2)

        del prev_list[:]
        add(prev_list, prev_if1)
        add(prev_list, prev_if2)
    
    return start;

def cfg_while(s, prev_list):
    start = Node(s, "while", prev_list)
    stats = s.statement()
    prev_if   = clone_list(prev_list)
    cfg_stmt(stats[0], prev_if)
    add(prev_list, prev_if)
    return start;

def cfg_expr_stmt(s, prev_list):    
    start = Node(s, expr_stmt2str(s), prev_list)    
    return start;

def cfg_emit(s, prev_list):
    start = Node(s, "emit", prev_list)    
    return start;

def cfg_transition(s, prev_list):
    start = Node(s, "transition", prev_list)    
    return start;

def cfg_assert(s, prev_list):
    start = Node(s, "assert", prev_list)    
    return start;

def cfg_wait(s, prev_list):
    start = Node(s, "wait", prev_list)    
    return start;

def cfg_after(s, prev_list):
    start = Node(s, "after", prev_list)    
    return start;


def cfg_stmt(s, prev_list):
    if s.if_stmt() != None:
        cfg_if(s.if_stmt(), prev_list);
    elif s.while_stmt() != None:
        cfg_while(s.while_stmt(), prev_list);
    elif s.expr_stmt() != None:
        cfg_expr_stmt(s.expr_stmt(), prev_list);
    elif s.transitionStatement() != None:
        cfg_transition(s.transitionStatement(), prev_list);
    elif s.block() != None:
        cfg_block(s.block(), prev_list);
    elif s.emit_stmt() != None:
        cfg_emit(s.emit_stmt(), prev_list);
    elif s.assert_stmt() != None:
        cfg_assert(s.assert_stmt(), prev_list);
    elif s.wait_stmt() != None:
        cfg_wait(s.wait_stmt(), prev_list);
    elif s.after_stmt() != None:
        cfg_after(s.after_stmt(), prev_list);
    else:
        print("---> don't know how to handle this statement");
        assert False;

    
def cfg_block(s, prev_list):
    start = Node(s, "block", prev_list);
    for n in s.statement():
        stmt = cfg_stmt(n, prev_list)
    return start


class CFG:
    def __init__(self, block, name):
        prev_list = []
        self.root = cfg_block(block, prev_list)
        self.write_dot("cfg-"+name+".dot")

        
    def write_dot(self, name):
        f = open(name, "w")
        f.write("digraph cfg {\n");
        seen = {}
        self.root.dot(f, seen)
        f.write("}\n")
        f.close()

        
