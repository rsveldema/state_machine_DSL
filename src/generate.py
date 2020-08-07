import sys;
from antlr4 import *
import pystache
from dslLexer import dslLexer
from dslParser import dslParser
from dslListener import dslListener
from analyzer import semantic_analysis,stateName2String,stateName2PrettyString,isExternal
from generate_c import generateC
from generate_st import generateST

currentMachine = None
event_list = []
test_suite_list = []
template_path="."


def write(f, s):
    f.write(s + "\n")

def writeMethod(self, str):
    self.code += str

class DottyGeneratorListener(dslListener):
    def __init__(self, fp):
        self.fp = fp
        self.currentState = None
        self.state_list = []
        
    def enterStateRule(self, ctxt):
        self.currentState = ctxt
        ctxt.all_transitions = []
        self.state_list.append(ctxt)
        
    def exitStateRule(self, ctxt):
        self.currentState = None

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
            fp.write(name + " [label=\""+pretty+"\"];\n")

            for t in p.all_transitions:  
                toName = stateName2String(t.stateName())
                fp.write(name + " -> " + toName + ";\n")

        if c != "":        
            fp.write("}\n")
            
    fp.write("}")
    fp.close()

class StringStream:
    def __init__(self):
        self.code = ""

    def write(self, s):
        self.code += s

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
generate_st = False
for arg in sys.argv[1:]:
    if arg.endswith(".sm"):
        fileName = arg
    elif arg.startswith('-path:'):
        template_path = arg[len('-path:'):]
    elif arg.startswith('-st'):
        generate_st = True
    else:
        Usage("unknown command line parameter: " + arg)

if fileName == None:
    Usage("missing xxx.sm file")

baseName = getBaseName(fileName)
tree = parse(fileName)
if semantic_analysis(tree, fileName):
    generateDotFile(tree, baseName)

    if generate_st:
        generateST(tree, fileName, baseName, template_path)
    else:
        generateC(tree, fileName, baseName, template_path)
        generate_test_harness(fileName, baseName)
else:
    sys.exit(1)
