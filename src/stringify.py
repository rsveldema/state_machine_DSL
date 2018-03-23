from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from antlr4 import *

def paren2str(p):
    return expr2str(p.expr())

def bin2str(p):
    if p.expr() == None:
        return lhs2str(p.lhs())
    else:
        ret = "("
        ret += lhs2str(p.lhs())
        ret += str(p.op)
        ret += expr2str(p.expr())
        
        ret += ")"
        return ret

def expr2str(s):
    # paren_expr | bin_expr | time_expr | NUMBER | STRING;
    if s.paren_expr() != None:
        return paren2str(s.paren_expr())
    elif s.bin_expr() != None:
        return bin2str(s.bin_expr())
    elif s.time_expr() != None:
        return paren2str(s.time_expr())
    elif s.NUMBER() != None:
        return str(s.NUMBER())
    elif s.STRING() != None:
        return str(s.STRING()).replace('\\n', '').replace('"', '\\"')
    
    return "expr"


def arglist2str(s):
    ret = "("
    comma = ""
    for e in s.expr():
        ret += comma + expr2str(e)
        comma = ", "        
    ret += ")"
    return ret

def selector2str(s):
    return s.op.text + str(s.ID())

def lhs2str(s):
    # ID selector* arg_list? ;
    ret = str(s.ID())
    for selector in s.selector():
        ret += selector2str(selector)
    if s.arg_list() != None:
        ret += arglist2str(s.arg_list())
    return ret

def expr_stmt2str(s):
    if s.expr() == None:
        return lhs2str(s.lhs());
    else:
        return lhs2str(s.lhs()) + s.op.text + expr2str(s.expr())


def auto_stmt2str(s):
    return "auto " + lhs2str(s.lhs()) + " = " + expr2str(s.expr())
