from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from antlr4 import *
from cfg import CFG

error_count = 0
filename = "<file>"



def error(stmt, msg):
    global error_count
    print(filename + ":" + str(stmt.start.line) + ": ERROR: " + msg)
    error_count += 1

def transition_visit(node, seen):
    if node in seen:
        return
    seen[node] = node

    if node.name == "transition":
        if len(node.next) > 0:
            error(node.stmt, "transition statement must be the last statement in a entry/exit/handler block")
    
    for n in node.next:
        transition_visit(n, seen)

def check_transition_stmts(cfg):
    seen = {}
    transition_visit(cfg.root, seen)


def stateName2StrInternal(stateName, selector):
    ret = ""
    sel = ""
    for i in stateName.ID():
        ret += sel + str(i)
        sel = selector
    return ret;
    
def stateName2PrettyString(stateName):
    return stateName2StrInternal(stateName, "::")

def stateName2String(stateName):
    return stateName2StrInternal(stateName, "_")
    
    
# Transition statements should:
#     - entry and exit statements should
#     - handler statements should contains transition statements only at the end of the method
class AnalyzerListener(dslListener):
    def __init__(self):
        self.name = ""

    def enterStateRule(self, ctxt):
        self.name = stateName2String(ctxt.stateName());
    
    def enterEntryBlock(self, ctxt):
        cfg = CFG(ctxt.block(), self.name + "-entry");
        check_transition_stmts(cfg)

    def enterExitBlock(self, ctxt):
        cfg = CFG(ctxt.block(), self.name + "-exit");
        check_transition_stmts(cfg)

    def enterEventHandler(self, ctxt):
        cfg = CFG(ctxt.block(), self.name + "-handler-" + str(ctxt.ID()))
        check_transition_stmts(cfg)


""" Returns true if everything OK
"""
def semantic_analysis(tree, _filename):
    global filename
    filename = _filename

    analyzer = AnalyzerListener()
    walker = ParseTreeWalker()
    walker.walk(analyzer, tree)
    return error_count == 0

    
