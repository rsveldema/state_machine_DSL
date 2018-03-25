import sys;
from antlr4 import *
import pystache
from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from analyzer import semantic_analysis,stateName2String,stateName2PrettyString,isExternal

currentMachine = None
event_list = []
test_suite_list = []
template_path="."


def write(f, s):
    f.write(s + "\n");

def writeMethod(self, str):
    self.code += str;

def generate_unary(f, e):
    write(f, str(e.op.text));
    generate_expr(f, e.expr())
    
def generate_paren(f, e):
    write(f, "(");
    generate_expr(f, e.expr())
    write(f, ")");

def generate_transition(f, t):
    name = stateName2String(t.stateName())
    write(f, "self->transition(self->state_union." + name + ");");

def generate_bin(f, e):
    lhs = e.lhs()
    generate_lhs(f, lhs)
    op = e.op
    if op != None:
        f.write(" " + str(op.text) + " ");
        generate_expr(f, e.expr())

def generate_if(f, s):
    write(f, "if (");
    generate_expr(f, s.expr());
    f.write(")");
    stat = s.statement();
    if len(stat) == 1:
        generate_stmt(f, stat[0])
    elif len(stat) == 2:
        generate_stmt(f, stat[0])
        f.write(" else ");
        generate_stmt(f, stat[1])        
    else:
        assert False


def generate_time(f, t):
    f.write("units::" + str(t.unit.text) + "(");
    generate_expr(f, t.expr());
    f.write(")");
        
def generate_expr(f, e):
    #expr: paren_expr | bin_expr | time_expr | NUMBER | STRING;
    if e.unary() != None:
        generate_unary(f, e.unary())
    elif e.paren_expr() != None:
        generate_paren(f, e.paren_expr());
    elif e.bin_expr() != None:
        generate_bin(f, e.bin_expr());
    elif e.time_expr() != None:
        generate_time(f, e.time_expr());
    elif e.NUMBER() != None:
        f.write(str(e.NUMBER()));
    elif e.STRING() != None:
        f.write(str(e.STRING()));
    else:
        assert False;
            

def generate_arg_list(f, arg_list):
    f.write("(");
    comma = ""
    for expr in arg_list.expr():
        f.write(comma)
        generate_expr(f, expr)
        comma = ", "        
    f.write(")");
    
def containsDecl(currentMachine, name):
    for cr in currentMachine.codeRule():
        dr = cr.declRule()
        if dr != None:
            names = dr.ID();
            declname = names[1]
            #print("TEST: " + str(declname));
            if name == str(declname):
                return True
    return False


def generate_lhs(f, lhs):    
    name = str(lhs.ID());
    if lhs.lhsPrefix() != None:
        f.write("::")
    elif containsDecl(currentMachine, name):
        f.write("self->");
    f.write(name);
    
    for selector in lhs.selector():
        f.write(str(selector.op.text) + str(selector.ID()));
            
    if lhs.arg_list() != None:
        generate_arg_list(f, lhs.arg_list())


def generate_auto(f, s):
    f.write("auto ");
    lhs = s.lhs()
    rhs = s.expr();
    generate_lhs(f, lhs)
    f.write(" = ");
    generate_expr(f, rhs)
    f.write(";\n")
    
        
def generate_expr_stmt(f, s):
    f.write("");
    lhs = s.lhs()
    rhs = s.expr();
    if rhs == None:
        generate_lhs(f, lhs)
    else:
        op = s.op;
        generate_lhs(f, lhs)
        f.write(" " + str(op.text) + " ");
        generate_expr(f, rhs)
    f.write(";\n")

    
def generate_after(f, after):
    write(f, "self->emit(self->" + str(after.ID()) + ", Timeout(");
    generate_expr(f, after.expr());
    f.write("));\n");


def generate_emit(f, s):
    write(f, "self->emit(self->" + str(s.ID()) + ");");


def generate_assert(f, s):
    f.write("ASSERT(");
    generate_expr(f, s.expr())
    f.write(");\n");

def generate_wait(f, s):
    f.write("WAIT(");
    generate_time(f, s.time_expr())
    f.write(");\n");
        
def generate_stmt(f, s):
    if s.if_stmt() != None:
        generate_if(f, s.if_stmt());
    elif s.while_stmt() != None:
        generate_while(f, s.while_stmt());
    elif s.expr_stmt() != None:
        generate_expr_stmt(f, s.expr_stmt());
    elif s.transitionStatement() != None:
        generate_transition(f, s.transitionStatement());
    elif s.block() != None:
        generate_block(f, s.block());
    elif s.emit_stmt() != None:
        generate_emit(f, s.emit_stmt());
    elif s.assert_stmt() != None:
        generate_assert(f, s.assert_stmt());
    elif s.wait_stmt() != None:
        generate_wait(f, s.wait_stmt());
    elif s.after_stmt() != None:
        generate_after(f, s.after_stmt());
    elif s.auto_stmt() != None:
        generate_auto(f, s.auto_stmt());
    else:
        print("---> don't know how to handle this statement");
        assert False;

def generate_block(f, blk):
    write(f, "{");
    for s in blk.statement():
        generate_stmt(f, s)
    write(f, "}");
    

def generate_eventHandler(f, c, name, machine_name, state, eventHandlerList):
    for eventHandler in eventHandlerList:
        event_name = str(eventHandler.ID())
        write(f, "void handler_" + event_name + "(BASE_"+str(currentMachine.ID())+" *self);");
        write(c, "void  BASE_"+machine_name+"::TYPE_"+name+"::handler_" + event_name + "(BASE_"+str(currentMachine.ID())+" *self) {");
        generate_block(c, eventHandler.block());
        write(c, "}");
        

def generate_entryBlock(f, c, name, machine_name, state, entryBlkList):
    state_name = stateName2String(state.stateName())
    write(f, "void entry(BASE_"+str(currentMachine.ID())+" *self);");
    write(c, "void  BASE_"+machine_name + "::TYPE_" + name+"::entry(BASE_"+str(currentMachine.ID())+" *self) {");
    write(c, "initialize_state(self, this, STATES::STATE_" + state_name + ");");
    for entryBlk in entryBlkList:
        generate_block(c, entryBlk.block());
    write(c, "}");
    
def generate_exitBlock(f, c, name, machine_name, state, exitBlkList):
    state_name = stateName2String(state.stateName())
    write(f, "void exit(BASE_"+str(currentMachine.ID())+" *self);");
    write(c, "void  BASE_"+machine_name + "::TYPE_" +name+"::exit(BASE_"+str(currentMachine.ID())+" *self) {");
    for exitBlk in exitBlkList:
        generate_block(c, exitBlk.block());
    write(c, "}");

def generate_declsBlock(f, c, name, machine_name, state, decls):
    f.write("std::tuple<");
    index = 0
    for d in decls:
        generate_machine_decl(f, c, d, index)
    write(f, "> fields;");

    index = 0
    for r in decls:
        generate_machine_decl_access(f, c, d, index)
        index += 1


def isInitialState(modlist):
    for m in modlist:
        return True
    return False

def generate_machine_state(f, c, machine_name, state, state_list):
    if state != None:
        name = stateName2String(state.stateName())

        # generate the class for this state:
        write(f, "class TYPE_" + name + " {");
        write(f, "public:");
        generate_eventHandler(f, c, name, machine_name, state, state.eventHandler());
        generate_entryBlock(f, c, name, machine_name, state, state.entryBlock());
        generate_declsBlock(f, c, name, machine_name, state, state.declRule());
        generate_exitBlock(f, c, name, machine_name, state, state.exitBlock());
        write(f, "};");

        write(f, "void transition(const TYPE_"+name+" &);");
        write(c, "void BASE_"+machine_name+"::transition(const TYPE_"+name+" &) {");
        write(c, "switch (state) {");
        write(c, "default: { STATE_MISSING_EVENT_HANDLER(\"none\", \"state\"); break; }");
        write(c, "case STATES::STATE_NONE: break;");
        for state in state_list:
            state_name = stateName2String(state.stateName())
            write(c, "case STATES::STATE_" + state_name + ": state_union."+state_name+".exit(this); break;");
        write(c, "}");
        write(c, "state_union." + name + ".entry(this);");
        write(c, "}");

        #print("initial state["+name+"]: " + str(state.stateModifier()))
        if True: #isInitialState(state.stateModifier()):
            write(f, "void initial_transition(const TYPE_"+name+" &);");
            write(c, "void BASE_"+machine_name+"::initial_transition(const TYPE_"+name+" &) {");
            write(c, "switch (state) {");
            write(c, "default: break; ");
            write(c, "case STATES::STATE_NONE: break;");
            for state in state_list:
                state_name = stateName2String(state.stateName())
                write(c, "case STATES::STATE_" + state_name + ": state_union."+state_name+".exit(this); break;");
            write(c, "}");
            write(c, "state_union." + name + ".entry(this);");
            write(c, "}");
                        

def generate_machine_state_enum(f, state, state_list):
    if state != None:
        state_list.append(state)
        name = stateName2String(state.stateName())
        write(f, "STATE_" + name + ",");

def generate_machine_event_enum(f, event, genString):
    global event_list;
    if event != None:
        name =  str(event.ID());
        if not genString:
            write(f, "case EVENT::EVENT_" + name + ": return \""+name+"\";");
        else:
            event_list.append(event)
            write(f, "EVENT_" + name + ",");

        
def generate_machine_event(f, c, event, events):
    if event != None:
        name = str(event.ID())
        write(f, "Event " + name + ";");
        if isExternal(event):
            write(events, "vec.add(" + name + ");")


def hasEventHandler(state, eventname):
    for h in state.eventHandler():
        name = str(h.ID())
        if name == eventname:
            return True
    return False

def generate_machine_event_handler(f, c, machinename, event, state_list):
    if event != None:
        eventname = str(event.ID())
        write(f, "void dispatch_" + eventname + "();");
        write(c, "void BASE_"+machinename+"::dispatch_" + eventname + "() {");
        write(c, "#if SUPPORT_MODEL_CHECKING");
        write(c, "setInstance(this);");
        write(c, "#endif");
        write(c, "switch (state) {");
        write(c, "default: { STATE_MISSING_EVENT_HANDLER(\"none\", \""+eventname+"\"); break; }");
        for state in state_list:
            statename = stateName2String(state.stateName())
            write(c, "case STATES::STATE_" + statename + ": {");
            if hasEventHandler(state, eventname):
                write(c, "state_union." + statename + ".handler_" + eventname + "(this);");
            else:
                pretty_name = stateName2PrettyString(state.stateName())
                write(c, "STATE_MISSING_EVENT_HANDLER(\"" + pretty_name + "\" , \"" + eventname + "\");");
            write(c, "break;");
            write(c, "}");
        write(c, "}");
        write(c, "}");


def generate_machine_decl_access(f, c, decl, index):
    if decl != None:
        names = decl.ID()
        write(f, str(names[0]) + " & " + str(names[1]) + " = std::get<"+str(index)+">(fields);")

def generate_machine_decl(f, c, decl, index):
    if decl != None:
        names = decl.ID()
        op = ""
        if decl.modifier != None:
            op += decl.modifier.text
        if (index > 0):
            f.write( "," + str(names[0])); # + " " + op + " " + str(names[1]) + ";")
        else:
            f.write(str(names[0])); # + " " + op + " " + str(names[1]) + ";")


def generate_events(f, c, codeRule, events):
    for r in codeRule:
        generate_machine_event(f, c, r.eventRule(), events)

def generate_event_handler(f, c, machinename, codeRule, state_list):
    for r in codeRule:
        generate_machine_event_handler(f, c, machinename, r.eventRule(), state_list)


def create_state_accessor(f, state_list, const):
    f.write("std::tuple<");
    comma=""
    for s in state_list:
        name = stateName2String(s.stateName())
        f.write(comma + const + " TYPE_" + name + "*");
        comma=","
    write(f, "> getStateVector() "+const+" {\n");
    write(f, "   switch (state) {\n");
    write(f, "default: assert(false);");
    ix = 0
    for s in state_list:
        name = stateName2String(s.stateName())
        write(f, "case STATES::STATE_" + name + ": {")
        f.write("return {");
        comma=""
        for p in range(0,ix):
            f.write(comma+"nullptr")
            comma=", "
        f.write(comma+"&state_union."+name);
        comma=", "
        for p in range(ix+1,len(state_list)):
            f.write(comma+"nullptr")
            comma=", "
        write(f, "};");
        write(f, "}");
        ix += 1
    f.write("   }\n");
    f.write("}\n");

        
def generate_states(f, c, machine_name, codeRule, state_list, enum_states_str):
    for r in codeRule:
        generate_machine_state(f, c, machine_name, r.stateRule(), state_list)
        
    write(f, "union StateUnion {\n");
    write(f, " StateUnion() {}");
    for s in state_list:
        name = stateName2String(s.stateName())
        write(f, "TYPE_" + name + " " + name + ";\n");
    write(f, "} state_union;\n\n");

    create_state_accessor(f, state_list, "const")    
    
    for s in state_list:
        name = stateName2String(s.stateName())
        pretty_name = stateName2PrettyString(s.stateName())        
        write(enum_states_str, "case STATES::STATE_" + name + ": {")
        write(enum_states_str, "   return \"" + pretty_name + "\";");
        write(enum_states_str, "}");
        


def generate_states_enum(f, codeRule, state_list):
    for r in codeRule:
        generate_machine_state_enum(f, r.stateRule(), state_list)

def generate_event_enum(f, codeRule, genString):
    for r in codeRule:
        generate_machine_event_enum(f, r.eventRule(), genString)

        
def generate_decls(f, c, codeRule):
    f.write("std::tuple<");
    index = 0
    for r in codeRule:
        if r.declRule() != None:
            generate_machine_decl(f, c, r.declRule(), index)
            index += 1
    write(f, "> fields;");

    index = 0
    for r in codeRule:
        if r.declRule() != None:
            generate_machine_decl_access(f, c, r.declRule(), index)
            index += 1


def generate_event_ctor_call(h, eventRule, first):
    prefix = ": " if first else ", "
    name = str(eventRule.ID())
    write(h, prefix + name + "(EVENT::EVENT_" + name + ")");
    

def generate_constructor(h, c, machineRule, name):
    first = True
    write(h, "BASE_" + name + "();");
    write(c, "BASE_" + name + "::BASE_" + name + "()");
    for r in machineRule.codeRule():
        e = r.eventRule()
        if e != None:
            generate_event_ctor_call(c, e, first);
            first = False
    write(c, "{");
    write(c, "#if SUPPORT_MODEL_CHECKING");
    write(c, "  setInstance(this);");
    write(c, "#endif");
    write(c, "}");


def generate_emit_dispatch(h,c, name):
    global event_list;
    write(h, "void do_emit(const Event &event);");
    write(c, "void BASE_"+name+"::do_emit(const Event &event) {");
    write(c, "#if SUPPORT_MODEL_CHECKING");
    write(c, "setInstance(this);");
    write(c, "#endif");
    write(c, "switch (event.getType()) {");
    write(c, "default: { STATE_BAD_EVENT_HANDLER(\"none\", \"event\"); break; }");
    for ev in event_list:
        eventname = str(ev.ID());
        write(c, "case EVENT::EVENT_"+eventname+": dispatch_"+eventname+"(); break;");
    write(c, "}");
    write(c, "}");


class CGeneratorListener(dslListener):
    def __init__(self, h, c, test, enums, events, enum_states_str, baseName):
        self.base_class = None
        self.enum_states_str = enum_states_str
        self.test = test
        self.events = events
        self.h = h
        self.c = c
        self.enums = enums
        self.baseName = baseName

    def enterTestsuiteRule(self, ctxt):
        global test_suite_list;
        
        suitename = str(ctxt.ID())
        smn = str(currentMachine.ID())
        test_suite_list.append(suitename)

        write(self.test, "namespace "+ suitename + "{");
        write(self.test, "static "+smn+" *self;\n");
        
        print("testsuite: " + suitename)
        for tr in ctxt.testRule():
            testname = str(tr.ID())
            write(self.test, "static void test_" + testname + "() {")
            for s in tr.statement():
                generate_stmt(self.test, s)
            write(self.test, "}\n\n");

        write(self.test, "static int initTests_"+smn+"() {");
        write(self.test, " static " + smn + " storage;")
        write(self.test, " self = new ((void*)&storage)" + smn + "();")
        write(self.test, " return CUE_SUCCESS;")
        write(self.test, "}")

        write(self.test, "static int cleanupTests_"+smn+"() {");        
        write(self.test, " self = NULL; // statically allocated object, no need to 'delete' it")
        write(self.test, " return CUE_SUCCESS;")
        write(self.test, "}")
        write(self.test, "};")
                

        write(self.test, "void registerTests_"+suitename+"() {");
        write(self.test, "auto suite = CREATE_TEST_SUITE("+suitename+"::initTests_"+smn+", "+suitename+"::cleanupTests_"+smn+", \"" + suitename + "\");");
        for tr in ctxt.testRule():
            testname = str(tr.ID())
            write(self.test, "REGISTER_TEST_IN_SUITE(suite, "+suitename+"::test_"+testname+", \""+testname+"\");");
        write(self.test, "REGISTER_TEST_SUITE(suite);");
        write(self.test, "}");

            
    def enterMachineRule(self, ctxt):
        global currentMachine
        currentMachine = ctxt
        
        name = str(ctxt.ID())

        state_list = []
        ctxt.state_list = state_list;

        for p in ctxt.machineProperty():
            if p.op.text == 'delayed_events':
                self.base_class = "DelayedEventsStateMachine"
            elif p.op.text == 'modelcheckable':
                self.base_class = "ModelCheckableStateMachine"
            else:
                print("don't know state machine property: " + p.op.text);
                sys.exit(1);
        
        write(self.enums, "namespace " + baseName + "{")
        write(self.enums, "enum class STATES {");
        write(self.enums, "STATE_NONE,");
        generate_states_enum(self.enums, ctxt.codeRule(), state_list)
        write(self.enums, "};");
        
        write(self.enums, "enum class EVENT {");
        write(self.enums, "EVENT_NONE,");
        generate_event_enum(self.enums, ctxt.codeRule(), True)
        write(self.enums, "};");

        write(self.enums, "static const char* eventToString(EVENT ev) {");
        write(self.enums, "   switch (ev) { ");
        write(self.enums, "   case EVENT::EVENT_NONE: return \"none\"; ");
        generate_event_enum(self.enums, ctxt.codeRule(), False)
        write(self.enums, "   }");
        write(self.enums, "}");
        write(self.enums, "}")

        # generate CTOR:
        generate_constructor(self.h, self.c, ctxt, name);
        generate_events(self.h, self.c, ctxt.codeRule(), self.events)
        write(self.h, "");
        generate_event_handler(self.h, self.c, name, ctxt.codeRule(), state_list)
        write(self.h, "");
        generate_decls(self.h, self.c, ctxt.codeRule())
        write(self.h, "");
        generate_states(self.h, self.c, name, ctxt.codeRule(), state_list, self.enum_states_str)
        generate_emit_dispatch(self.h, self.c, name);
        

class DottyGeneratorListener(dslListener):
    def __init__(self, fp):
        self.fp = fp
        self.currentState = None
        self.state_list = []
        
    def enterStateRule(self, ctxt):
        self.currentState = ctxt;
        ctxt.all_transitions = []
        self.state_list.append(ctxt)
        
    def exitStateRule(self, ctxt):
        self.currentState = None;

    def enterTransitionStatement(self, ctxt):
        if self.currentState != None:
            self.currentState.all_transitions.append(ctxt)         

def add_to_container(containers, p):
    names = p.stateName().ID()
    if len(names) == 1:
        c = ""
    else:
        c = str(names[0])
        
    if not c in containers:
        containers[c] = []
    containers[c].append(p)
    
            
def generateDotFile(tree, baseName):
    fp = open(baseName + ".dot", "w")
    fp.write("digraph file {\n");   
    
    listener = DottyGeneratorListener(fp)
    walker = ParseTreeWalker()
    walker.walk(listener, tree)

    containers={}
    for p in listener.state_list:
        add_to_container(containers, p)

    for c in containers:
        if c != "":        
            fp.write("subgraph cluster_" + c + " {\n")
            fp.write("\tlabel = \""+c+"\"\n");
            fp.write("\tlabeljust=\"l\";\n")
            
        plist = containers[c]
        for p in plist:
            name = stateName2String(p.stateName())
            pretty = stateName2PrettyString(p.stateName())
            fp.write(name + " [label=\""+pretty+"\"];\n");

            for t in p.all_transitions:  
                toName = stateName2String(t.stateName())
                fp.write(name + " -> " + toName + ";\n");

        if c != "":        
            fp.write("}\n")
            
    fp.write("}");
    fp.close();

class StringStream:
    def __init__(self):
        self.code = ""

    def write(self, s):
        self.code += s;


def generateC(tree, fileName, baseName):
    test = open("generated_test_"+baseName+".cc", "w")
    test.write("#include \"generated_state_machine_"+baseName+".hpp\"\n");
    test.write("// generated from " + fileName + "\n");
    
    h          = StringStream()
    c          = StringStream()
    enums      = StringStream()
    events     = StringStream()
    enum_states_str = StringStream()
    
    printer = CGeneratorListener(h, c, test, enums, events, enum_states_str, baseName)
    walker  = ParseTreeWalker()
    walker.walk(printer, tree)

    write(test, "void registerTests_"+baseName+"() {");
    for suitename in test_suite_list:
        write(test, "\tregisterTests_"+suitename+"();");
    write(test, "}");
    test.close()


    names = {}
    names['USE_FEATURE_CLASS'] = "0" if printer.base_class == None else "1"
    names['base_class'] = printer.base_class
    names['base_name'] = baseName
    names['state_machine_name'] = str(currentMachine.ID())
    names['MAIN_CODE'] = c.code
    names['MAIN_DECL'] = h.code
    names['ENUM_CODE'] = enums.code
    names['EVENT_VEC'] = events.code
    names['STATE2STR'] = enum_states_str.code
    
    with open(template_path + '/template.generated_code.hpp') as x: f = x.read()
    h = open("generated_state_machine_"+baseName+".hpp", "w")
    rendered = pystache.Renderer().render(f, names)
    h.write(rendered)
    h.close()

    with open(template_path + '/template.generated_code.cpp') as x: f = x.read()
    h = open("generated_state_machine_"+baseName+".cpp", "w")
    rendered = pystache.Renderer().render(f, names)
    h.write(rendered)
    h.close()

    with open(template_path + '/model_check.template.cc') as x: f = x.read()
    h = open("model_check_"+baseName+".cc", "w")
    rendered = pystache.Renderer().render(f, names)
    h.write(rendered)
    h.close()



def parse(fileName):
    print("Reading " + fileName)

    input = FileStream(fileName)
    lexer = dslLexer(input)
    stream = CommonTokenStream(lexer)
    parser = dslParser(stream)
    tree = parser.startRule()
    return tree;

def getBaseName(name):
    ix = name.rfind('/')
    if ix >= 0:
        name = name[ix+1:]
    ix = name.find('.')
    if ix >= 0:
        name = name[0:ix]
    return name


def generate_test_harness(fileName, baseName):    
    with open(template_path + '/main.template.cc') as x: f = x.read()
    
    names = {}
    names['base_name'] = baseName
    names['state_machine_name'] = str(currentMachine.ID())
    rendered = pystache.render(f, names)
    
    fp = open('main_' + baseName + ".cc", 'w')
    fp.write(rendered)
    fp.close()

def Usage(msg):
    print(msg)
    sys.exit(1)
    
    
pystache.defaults.TAG_ESCAPE = lambda u: u
fileName = None
for arg in sys.argv[1:]:
    if arg.endswith(".sm"):
        fileName = arg;
    elif arg.startswith('-path:'):
        template_path = arg[len('-path:'):]
    else:
        Usage("unknown command line parameter: " + arg)

if fileName == None:
    Usage("missing xxx.sm file")

baseName = getBaseName(fileName)
tree = parse(fileName)
if semantic_analysis(tree, fileName) == True:
    generateDotFile(tree, baseName)
    generateC(tree, fileName, baseName)
    generate_test_harness(fileName, baseName)
else:
    sys.exit(1)
