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
    if e.paren_expr() != None:
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
    write(f, "self->emit(self->" + str(after.ID()) + ", ZEP::Utilities::Timeout(");
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
    else:
        print("---> don't know how to handle this statement");
        assert False;

def generate_block(f, blk):
    write(f, "{");
    for s in blk.statement():
        generate_stmt(f, s)
    write(f, "}");
    

def generate_eventHandler(f, eventHandlerList):
    for eventHandler in eventHandlerList:
        event_name = str(eventHandler.ID())
        write(f, "void handler_" + event_name + "("+str(currentMachine.ID())+" *self) {");
        write(f, "SM_TRACE(\"TRIGGER EVENT: " + event_name + "\");");
        generate_block(f, eventHandler.block());
        write(f, "}");
        

def generate_entryBlock(f, state, entryBlkList):
    state_name = stateName2String(state.stateName())
    write(f, "void entry("+str(currentMachine.ID())+" *self) {");
    write(f, "memset(this, 0, sizeof(*this));");
    write(f, "self->state = STATES::STATE_" + state_name + ";");
    write(f, "SM_TRACE(\"ENTER STATE: " + state_name + "\");");
    for entryBlk in entryBlkList:
        generate_block(f, entryBlk.block());
    write(f, "}");
    
def generate_exitBlock(f, state, exitBlkList):
    write(f, "void exit("+str(currentMachine.ID())+" *self) {");
    for exitBlk in exitBlkList:
        generate_block(f, exitBlk.block());
    write(f, "}");

def generate_declsBlock(f, state, decls):
    for d in decls:
        generate_machine_decl(f, d, None, None, None)

    name = stateName2String(state.stateName())
    write(f, "bool operator < (const TYPE_" + name + " &other) const {");
    for d in decls:
        decl_name = str(d.ID()[1])
        write(f, "if (" + decl_name + " < other." + decl_name + ") {");
        #write(f, "   LOG_DEBUG(\" \");");
        write(f, "   return true;");
        write(f, "}");
    write(f, "return false;");
    write(f, "}");

    write(f, "bool operator == (const TYPE_" + name + " &other) const {");
    for d in decls:
        decl_name = str(d.ID()[1])
        write(f, "if (" + decl_name + " != other." + decl_name + ") {");
        write(f, "   return false;");
        write(f, "}");
    write(f, "return true;");
    write(f, "}");

    write(f, "std::string toString () const {");
    write(f, "std::string ret(\"STATE:" + name + "\");");
    for d in decls:
        decl_name = str(d.ID()[1])
        write(f, "ret += \"," + decl_name + "=\" + convertToString(" + decl_name + ");");
    write(f, "return ret;");
    write(f, "}");


def isInitialState(modlist):
    for m in modlist:
        return True
    return False

def generate_machine_state(f, state, state_list):
    if state != None:
        name = stateName2String(state.stateName())
        
        write(f, "class TYPE_" + name + " {");
        write(f, "public:");
        generate_eventHandler(f, state.eventHandler());
        generate_entryBlock(f, state, state.entryBlock());
        generate_declsBlock(f, state, state.declRule());
        generate_exitBlock(f, state, state.exitBlock());
        write(f, "};");

        write(f, "void transition(const TYPE_"+name+" &) {");
        write(f, "switch (state) {");
        write(f, "default: { STATE_MISSING_EVENT_HANDLER(\"none\", \"state\"); break; }");
        write(f, "case STATES::STATE_NONE: break;");
        for state in state_list:
            state_name = stateName2String(state.stateName())
            write(f, "case STATES::STATE_" + state_name + ": state_union."+state_name+".exit(this); break;");
        write(f, "}");
        write(f, "state_union." + name + ".entry(this);");
        write(f, "}");

        print("initial state["+name+"]: " + str(state.stateModifier()))
        if True: #isInitialState(state.stateModifier()):
            write(f, "void initial_transition(const TYPE_"+name+" &) {");
            write(f, "switch (state) {");
            write(f, "default: break; ");
            write(f, "case STATES::STATE_NONE: break;");
            for state in state_list:
                state_name = stateName2String(state.stateName())
                write(f, "case STATES::STATE_" + state_name + ": state_union."+state_name+".exit(this); break;");
            write(f, "}");
            write(f, "state_union." + name + ".entry(this);");
            write(f, "}");
                        

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

        
def generate_machine_event(f, event, events):
    if event != None:
        name = str(event.ID())
        write(f, "Event " + name + ";");
        if isExternal(event):
            write(events, "vec.push_back(" + name + ");")


def hasEventHandler(state, eventname):
    for h in state.eventHandler():
        name = str(h.ID())
        if name == eventname:
            return True
    return False

def generate_machine_event_handler(f, event, state_list):
    if event != None:
        eventname = str(event.ID())
        write(f, "void dispatch_" + eventname + "() {");
        write(f, "switch (state) {");
        write(f, "default: { STATE_MISSING_EVENT_HANDLER(\"none\", \""+eventname+"\"); break; }");
        for state in state_list:
            statename = stateName2String(state.stateName())
            write(f, "case STATES::STATE_" + statename + ": {");
            if hasEventHandler(state, eventname):
                write(f, "state_union." + statename + ".handler_" + eventname + "(this);");
            else:
                pretty_name = stateName2PrettyString(state.stateName())
                write(f, "STATE_MISSING_EVENT_HANDLER(\"" + pretty_name + "\" , \"" + eventname + "\");");
            write(f, "break;");
            write(f, "}");
        write(f, "}");

        write(f, "}");

def generate_machine_decl(f, decl, hashmethod, compare, equal_compare):
    if decl != None:
        names = decl.ID()
        write(f, str(names[0]) + " " + str(names[1]) + ";")

        if compare != None:
            write(compare, "if (" + str(names[1]) + " < other." + str(names[1])+  ") return true;");
            
        if equal_compare != None:
            write(equal_compare, "if (" + str(names[1]) + " != other." + str(names[1])+  ") return false;");
            
        if hashmethod != None:
            write(hashmethod, "hashValue.add(" + str(names[1]) + ".getHash());");

def generate_events(f, codeRule, events):
    for r in codeRule:
        generate_machine_event(f, r.eventRule(), events)

def generate_event_handler(f, codeRule, state_list):
    for r in codeRule:
        generate_machine_event_handler(f, r.eventRule(), state_list)
        
def generate_states(f, codeRule, state_list, states_compare, equal_states_compare, states_str):
    for r in codeRule:
        generate_machine_state(f, r.stateRule(), state_list)
        
    f.write("union {\n");
    for s in state_list:
        name = stateName2String(s.stateName())
        f.write("TYPE_" + name + " " + name + ";\n");
    f.write("} state_union;\n\n");

    for s in state_list:
        name = stateName2String(s.stateName())
        write(states_compare, "case STATES::STATE_" + name + ": {")
        write(states_compare, "   if (state_union." + name + " < other.state_union." + name + ") {");
        write(states_compare, "       return true;");
        write(states_compare, "   }");
        write(states_compare, "   break;");
        write(states_compare, "}");

        write(equal_states_compare, "case STATES::STATE_" + name + ": {")
        write(equal_states_compare, "   if (state_union." + name + " == other.state_union." + name + ") {");
        write(equal_states_compare, "       return true;");
        write(equal_states_compare, "   }");
        write(equal_states_compare, "   break;");
        write(equal_states_compare, "}");

        write(states_str, "case STATES::STATE_" + name + ": {")
        write(states_str, "   ret += state_union." + name + ".toString();");
        write(states_str, "   break;");
        write(states_str, "}");


def generate_states_enum(f, codeRule, state_list):
    for r in codeRule:
        generate_machine_state_enum(f, r.stateRule(), state_list)

def generate_event_enum(f, codeRule, genString):
    for r in codeRule:
        generate_machine_event_enum(f, r.eventRule(), genString)

        
def generate_decls(f, codeRule, hashmethod, compare, equal_compare):
    for r in codeRule:
        generate_machine_decl(f, r.declRule(), hashmethod, compare, equal_compare)

def generate_event_ctor_call(h, eventRule, first):
    prefix = ": " if first else ", "
    name = str(eventRule.ID())
    write(h, prefix + name + "(EVENT::EVENT_" + name + ")");
    

def generate_constructor(h, machineRule):
    first = True
    for r in machineRule.codeRule():
        e = r.eventRule()
        if e != None:
            generate_event_ctor_call(h, e, first);
            first = False

class CGeneratorListener(dslListener):
    def __init__(self, h, test, enums, hashmethod, events, compare, states_compare, equal_compare, equal_states_compare, states_str):
        self.states_str = states_str;
        self.equal_compare = equal_compare
        self.equal_states_compare = equal_states_compare
        self.test = test
        self.states_compare = states_compare
        self.compare = compare
        self.events = events
        self.h = h
        self.enums = enums
        self.hashmethod = hashmethod;

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
          
        write(self.enums, "enum class STATES {");
        write(self.enums, "STATE_NONE,");
        generate_states_enum(self.enums, ctxt.codeRule(), state_list)
        write(self.enums, "} state;");
        
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

        # generate CTOR:
        write(self.h, name + "()");
        generate_constructor(self.h, ctxt);
        write(self.h, "{}");

        generate_events(self.h, ctxt.codeRule(), self.events)
        write(self.h, "");
        generate_event_handler(self.h, ctxt.codeRule(), state_list)
        write(self.h, "");
        generate_decls(self.h, ctxt.codeRule(), self.hashmethod, self.compare, self.equal_compare)
        write(self.h, "");
        generate_states(self.h, ctxt.codeRule(), state_list, self.states_compare, self.equal_states_compare, self.states_str)

        write(self.h, "void do_emit(const Event &event) {");
        write(self.h, "switch (event.getType()) {");
        write(self.h, "default: { STATE_BAD_EVENT_HANDLER(\"none\", \"event\"); break; }");
        for ev in event_list:
            eventname = str(ev.ID());
            write(self.h, "case EVENT::EVENT_"+eventname+": dispatch_"+eventname+"(); break;");
        write(self.h, "}");
        write(self.h, "}");
        

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
    hashmethod = StringStream()
    enums      = StringStream()
    events     = StringStream()
    compare    = StringStream()
    states_compare = StringStream()
    states_str = StringStream()
    equal_compare    = StringStream()
    equal_states_compare = StringStream()

    printer = CGeneratorListener(h, test, enums, hashmethod, events, compare, states_compare, equal_compare, equal_states_compare, states_str)
    walker  = ParseTreeWalker()
    walker.walk(printer, tree)

    write(test, "void registerTests_"+baseName+"() {");
    for suitename in test_suite_list:
        write(test, "\tregisterTests_"+suitename+"();");
    write(test, "}");
    test.close()


    with open(template_path + '/template.generated_code.h') as x: f = x.read()    
    names = {}
    names['base_name'] = baseName
    names['state_machine_name'] = str(currentMachine.ID())
    names['MAIN_CODE'] = h.code
    names['ENUM_CODE'] = enums.code
    names['HASH_CODE'] = hashmethod.code
    names['COMPARE_FIELDS'] = compare.code
    names['COMPARE_STATES'] = states_compare.code
    names['EQUAL_FIELDS'] = equal_compare.code
    names['EQUAL_STATES'] = equal_states_compare.code
    names['EVENT_VEC'] = events.code
    names['STATES_TO_STRING'] = states_str.code
    rendered = pystache.Renderer().render(f, names)
    
    h = open("generated_state_machine_"+baseName+".hpp", "w")
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
