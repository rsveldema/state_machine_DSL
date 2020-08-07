import sys;
from antlr4 import *
import pystache
from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from analyzer import semantic_analysis,stateName2String,stateName2PrettyString,isExternal
from dslListener import dslListener


indent_level = 0

def indent(i):
    global indent_level
    indent_level += i

def emit_indent(f):
    global indent_level
    t = ""
    for _ in range(0, indent_level):
        t += "\t"    
    f.write(t)

def writeln(f, s):
    emit_indent(f)
    f.write(s + "\n")

def write(f, s):
    emit_indent(f)
    f.write(s)

def writeMethod(self, str):
    self.code += str



def generate_machine_state_enum(f, state, state_list, counters):
    if state != None:
        state_list.append(state)
        name = stateName2String(state.stateName())
        writeln(f, "\t, ENTER_STATE_" + name + " := " + str(counters[0]))
        counters[0] += 1 
        writeln(f, "\t, STATE_" + name + " := " + str(counters[0]))
        counters[0] += 1 
        writeln(f, "\t, EXIT_STATE_" + name + " := " + str(counters[0]))
        counters[0] += 1 

def isInitialState(modlist):
    for _ in modlist:
        return True
    return False

def generate_states_enum(f, codeRule, state_list, counters):
    initial_state = ""
    for r in codeRule:
        state = r.stateRule()
        if state != None:
            if isInitialState(state.stateModifier()):
                initial_state = stateName2String(state.stateName())

        generate_machine_state_enum(f, r.stateRule(), state_list, counters)
    return initial_state



def generate_event_enum(f, codeRule, counters, event_list):
    for r in codeRule:
        event = r.eventRule()
        if event != None:
            name = str(event.ID())
            writeln(f, "\t, EVENT_" + name + " := " + str(counters[0]))
            counters[0] += 1 
            event_list.append(name)




def translate2chrono(s):
    prefix = "std::chrono::"
    if s == "millis":
        s = "milliseconds"
    elif s == "micros":
        s = "microseconds"
    elif s == "secs":
          s = "seconds"
    return prefix + s

def generate_time(f, t):
    f.write(translate2chrono(t.unit.text) + "(")
    generate_expr(f, t.expr())
    f.write(")")

def generate_if(f, s, ctxt):    
    write(f, "IF (")
    generate_expr(f, s.expr())
    f.write(") THEN")
    stat = s.statement()
    if len(stat) == 1:
        generate_stmt(f, stat[0], ctxt)
    elif len(stat) == 2:
        generate_stmt(f, stat[0], ctxt)
        writeln(f, "ELSE")
        generate_stmt(f, stat[1], ctxt)        
    else:
        assert False
    writeln(f, "END_IF")

def generate_while(f, stmt, ctxt):
    writeln(f, "UNIMPLEMENTED: WHILE")

    
def generate_unary(f, e):
    writeln(f, str(e.op.text))
    generate_expr(f, e.expr())
    
def generate_paren(f, e):
    writeln(f, "(")
    generate_expr(f, e.expr())
    writeln(f, ")")

def generate_bin(f, e):
    lhs = e.lhs()
    generate_lhs(f, lhs)
    op = e.op
    if op != None:
        f.write(" " + str(op.text) + " ")
        generate_expr(f, e.expr())

def generate_expr(f, e):
    #expr: paren_expr | bin_expr | time_expr | NUMBER | STRING;
    if e.unary() != None:
        generate_unary(f, e.unary())
    elif e.paren_expr() != None:
        generate_paren(f, e.paren_expr())
    elif e.bin_expr() != None:
        generate_bin(f, e.bin_expr())
    elif e.time_expr() != None:
        generate_time(f, e.time_expr())
    elif e.NUMBER() != None:
        f.write(str(e.NUMBER()))
    elif e.STRING() != None:
        f.write('str := ' + str(e.STRING()))
    else:
        assert False

def generate_arg_list(f, arg_list):
    f.write("(")
    comma = ""
    for expr in arg_list.expr():
        f.write(comma)
        generate_expr(f, expr)
        comma = ", "        
    f.write(")")
    

def containsDecl(currentMachine, name):
    for cr in currentMachine.codeRule():
        dr = cr.declRule()
        if dr != None:
            names = dr.ID()
            declname = names[1]
            #print("TEST: " + str(declname));
            if name == str(declname):
                return True
    return False

def generate_lhs(f, lhs):    
    name = str(lhs.ID())
    if lhs.lhsPrefix() != None:
        f.write(".")
    elif containsDecl(currentMachine, name):
        f.write("")
    f.write(name)
    
    for selector in lhs.selector():
        f.write(str(selector.op.text) + str(selector.ID()))
            
    if lhs.arg_list() != None:
        generate_arg_list(f, lhs.arg_list())

def generate_expr_stmt(f, stmt, ctxt):
    write(f, "")
    lhs = stmt.lhs()
    rhs = stmt.expr()
    if rhs == None:
        generate_lhs(f, lhs)
    else:
        op = stmt.op
        generate_lhs(f, lhs)
        f.write(" " + str(op.text) + " ")
        generate_expr(f, rhs)
    f.write(";\n")

def generate_transition(f, stmt, ctxt):    
    name = stateName2String(stmt.stateName())
    writeln(f, "state := StateMachineStateNames.EXIT_STATE_" + ctxt.current_state + ";")
    writeln(f, "next_state := StateMachineStateNames.ENTER_STATE_" + name + ";")

def generate_emit(f, stmt, ctxt):
    writeln(f, "events[EventType.EVENT_" + str(stmt.ID()) + "] := TRUE;")

def generate_assert(f, stmt, ctxt):
    writeln(f, "UNIMPL: assert")

def generate_wait(f, stmt, ctxt):
    writeln(f, "UNIMPL: wait")

def generate_after(f, stmt, ctxt):
    writeln(f, "UNIMPL: after")

def generate_auto(f, stmt, ctxt):
    writeln(f, "UNIMPL: auto")

def generate_stmt(f, s, ctxt):    
    if s.if_stmt() != None:
        generate_if(f, s.if_stmt(), ctxt)
    elif s.while_stmt() != None:
        generate_while(f, s.while_stmt(), ctxt)
    elif s.expr_stmt() != None:
        generate_expr_stmt(f, s.expr_stmt(), ctxt)
    elif s.transitionStatement() != None:
        generate_transition(f, s.transitionStatement(), ctxt)
    elif s.block() != None:
        generate_block(f, s.block(), ctxt)
    elif s.emit_stmt() != None:
        generate_emit(f, s.emit_stmt(), ctxt)
    elif s.assert_stmt() != None:
        generate_assert(f, s.assert_stmt(), ctxt)
    elif s.wait_stmt() != None:
        generate_wait(f, s.wait_stmt(), ctxt)
    elif s.after_stmt() != None:
        generate_after(f, s.after_stmt(), ctxt)
    elif s.auto_stmt() != None:
        generate_auto(f, s.auto_stmt(), ctxt)
    else:
        print("---> don't know how to handle this statement")
        assert False

def generate_block(f, blk, ctxt):
    indent(1)
    #write(f, "// BEGIN")
    for s in blk.statement():
        generate_stmt(f, s, ctxt)
    #write(f, "// END")
    indent(-1)
    

def generate_enter_state_code(f, state, ctxt):
    eblks = state.entryBlock()
    for blk in eblks:
        generate_block(f, blk.block(), ctxt)

def generate_exit_state_code(f, state, ctxt):
    eblks = state.exitBlock()
    for blk in eblks:
        generate_block(f, blk.block(), ctxt)


def generate_event_state_code(f, state, ctxt):
    eblks = state.eventHandler()
    ret = False
    for blk in eblks:
        ret = True
        event = str(blk.ID())
        writeln(f, "IF events[EventType.EVENT_"+event+"] THEN")
        indent(1)
        writeln(f, "events[EventType.EVENT_"+event+"] := FALSE;")
        indent(-1)
        generate_block(f, blk.block(), ctxt)
        writeln(f, "END_IF")
    return ret


def generate_switch_case(f, state, ctxt):
    name = stateName2String(state.stateName())
    ctxt.current_state = name
    writeln(f, "StateMachineStateNames.ENTER_STATE_" + name + ": ")
    writeln(f, "\tstate_statistics[StateMachineStateNames.ENTER_STATE_" + name + "] := state_statistics[StateMachineStateNames.ENTER_STATE_" + name + "] + 1;")
    generate_enter_state_code(f, state, ctxt)
    indent(1)
    writeln(f, "IF next_state = StateMachineStateNames.STATE_NONE THEN")
    indent(1)
    writeln(f, "state := " + "StateMachineStateNames.STATE_" + name + ";") 
    indent(-1)
    writeln(f, "END_IF")
    indent(-1)
    writeln(f, "")
    writeln(f, "")

    writeln(f, "StateMachineStateNames.STATE_" + name + ": ")
    indent(1)
    if not generate_event_state_code(f, state, ctxt):
        writeln(f, ";")
    indent(-1)
    writeln(f, "")
    writeln(f, "")


    writeln(f, "StateMachineStateNames.EXIT_STATE_" + name + ": ")
    generate_exit_state_code(f, state, ctxt)
    indent(1)
    writeln(f, "state := next_state;")
    writeln(f, "next_state := StateMachineStateNames.STATE_NONE;")
    indent(-1)
    writeln(f, "")
    writeln(f, "")
    

def generate_switch(f, codeRule, initial_state, ctxt):
    writeln(f, "CASE state OF ")
    writeln(f, "StateMachineStateNames.STATE_NONE:")
    indent(1)
    writeln(f, "state := StateMachineStateNames.ENTER_STATE_" + initial_state + ";")
    for e in ctxt.event_list:
        writeln(f, "events[EventType.EVENT_"+e+"] := FALSE;")
    indent(-1)
    writeln(f, "")
    writeln(f, "")

    for r in codeRule:
        state = r.stateRule()
        if state != None:
            generate_switch_case(f, r.stateRule(), ctxt)

    writeln(f, "END_CASE")

def generate_decls(f, codeRule, parse_ctxt):
    indent(1)
    for r in codeRule:
        decl = r.declRule()
        if decl != None:
            names = decl.ID()
            typename = names[0]
            varname = names[1]
            writeln(f, str(varname) + " : " + str(typename) + ";")
    indent(-1)


class ParseCtxt:
    def __init__(self, event_list):
        self.parse_ctxt = 0
        self.event_list = event_list

class STGeneratorListener(dslListener):
    def __init__(self, template_path):
        self.template_path = template_path
            
    def enterMachineRule(self, ctxt):
        global currentMachine
        currentMachine = ctxt        

        #name = str(ctxt.ID())
        state_list = []
        ctxt.state_list = state_list

        counters = [1]

        enums = open("tmp/states_enum.st", "w")
        writeln(enums, "TYPE StateMachineStateNames :")
        writeln(enums, "(")
        writeln(enums, "\tSTATE_NONE := 0")
        initial_state = generate_states_enum(enums, ctxt.codeRule(), state_list, counters)
        writeln(enums, "\t, MAX_STATE := " + str(counters[0]))
        writeln(enums, ");")
        writeln(enums, "END_TYPE")        
        enums.close()

        counters = [1]
        event_list = []
        enums = open("tmp/events_enum.st", "w")
        writeln(enums, "TYPE EventType :")
        writeln(enums, "(")
        writeln(enums, "\tEVENT_NONE := 0")
        generate_event_enum(enums, ctxt.codeRule(), counters, event_list)
        writeln(enums, "\t, MAX_EVENT := " + str(counters[0]))
        writeln(enums, ");")
        writeln(enums, "END_TYPE")        
        enums.close()

        switch = open("tmp/switch_statement.st", "w")
        parse_ctxt = ParseCtxt(event_list)
        generate_switch(switch, ctxt.codeRule(), initial_state, parse_ctxt)
        switch.close()

        decls = open("tmp/decls.st", "w")
        parse_ctxt = ParseCtxt(event_list)
        generate_decls(decls, ctxt.codeRule(), parse_ctxt)
        decls.close()

        with open('tmp/switch_statement.st') as x: switch = x.read()
        with open('tmp/decls.st') as x: decls = x.read()
        with open('tmp/states_enum.st') as x: states_enum = x.read()
        with open('tmp/events_enum.st') as x: events_enum = x.read()

        names = {}
        names['switch_code'] = switch
        names['decls'] = decls
        names['states_enum'] = states_enum
        names['events_enum'] = events_enum

        with open(self.template_path + '/main.template.st') as x: f = x.read()
        h = open("generated_main.st", "w")
        rendered = pystache.Renderer().render(f, names)
        h.write(rendered)
        h.close()

        with open(self.template_path + '/state_enum.template.st') as x: f = x.read()
        h = open("generated_states_enum.st", "w")
        rendered = pystache.Renderer().render(f, names)
        h.write(rendered)
        h.close()

        with open(self.template_path + '/event_enum.template.st') as x: f = x.read()
        h = open("generated_event_enum.st", "w")
        rendered = pystache.Renderer().render(f, names)
        h.write(rendered)
        h.close()

def generateST(tree, fileName, baseName, template_path):
    printer = STGeneratorListener(template_path)
    walker  = ParseTreeWalker()
    walker.walk(printer, tree)
