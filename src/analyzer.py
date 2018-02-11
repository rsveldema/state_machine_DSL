from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from antlr4 import *
from cfg import CFG

currentMachine = None
error_count = 0
filename = "<file>"



def error(stmt, msg):
    global error_count
    print(filename + ":" + str(stmt.start.line) + ": ERROR: " + msg)
    error_count += 1

    
def isExternal(event):
    modifiers = event.eventModifier()
    for m in modifiers:
        name = m.op.text
        if name == 'internal':
            return False;
    return True

    
def isInternal(event):
    modifiers = event.eventModifier()
    for m in modifiers:
        name = m.op.text
        if name == 'internal':
            return True;
    return False


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


def isEventName(eventName):
    global currentMachine
    for p in currentMachine.codeRule():
        event = p.eventRule()
        if event != None:
            name = event.ID()
            if str(name) == eventName:
                return True;
    return False
    
# Transition statements should:
#     - entry and exit statements should
#     - handler statements should contains transition statements only at the end of the method
class AnalyzerListener(dslListener):
    def __init__(self):
        self.name = ""

    def enterMachineRule(self, ctxt):
        global currentMachine
        currentMachine = ctxt

    def enterStateRule(self, ctxt):
        self.name = stateName2String(ctxt.stateName());

    def enteremit_stmt(self, ctxt):
        # can only trigger internal events
        name = ctxt.ID()
    
    def enterEntryBlock(self, ctxt):
        cfg = CFG(ctxt.block(), self.name + "-entry");
        check_transition_stmts(cfg)

    def enterExitBlock(self, ctxt):
        cfg = CFG(ctxt.block(), self.name + "-exit");
        check_transition_stmts(cfg)

    def enterEventHandler(self, ctxt):
        eventName = str(ctxt.ID())
        if not isEventName(eventName):
            error(ctxt, eventName + " is not the name of a declared event")
            
        cfg = CFG(ctxt.block(), self.name + "-handler-" + eventName)
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

    
