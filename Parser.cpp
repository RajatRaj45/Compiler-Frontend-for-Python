#include <bits/stdc++.h>
using namespace std;
#define ll long long

vector<string> tokens; 
vector<string> lexemes; 
int ptr = 0;

void error(string s){
    cout<<"PARSING ERROR! MESSAGE: "<<s<<"\n";
    // cout<<"ERROR FOR "<<ptr<<" "<<tokens[ptr]<<" "<<tokens.size()<<"\n";
    exit(0);
}



// ADJ[1] -> 
map<int, vector<int>> adj;
map<int, string> NodeInfo;
int currNum = 0;


// void dfs(int node, map<int,vector<int>> &adj){
//     // cout<<node<<" "<<NodeInfo[node]<<"\n";
//     for(auto el:adj[node]){
//         dfs(el,adj);
//     }
// }

// STRUCTS
struct ModuleNode;
struct ProgramNode;
struct BlockNode;
struct BlocksNode;
struct ForNode;
struct WhileNode;
struct IfNode;
struct PrintNode;
struct CondnNode;
struct ExprNode;
struct StatementNode;
struct FCallNode;
struct ValueNode;
struct FDefNode;
struct AssignNode;

struct ProgramNode{
    vector<ModuleNode*> modules;
    vector<int> numModules;
};

struct ModuleNode{
    //TYPE 0->Block
    //Type 1->FDef
    bool type;
    BlockNode *blockPtr;
    FDefNode *fDefPtr;
};

struct BlockNode{
    int type;
    //TYPE -> FOR 0, WHILE 1, IF 2, PRINT 3, STATEMENT 4
    ForNode *forPtr;
    WhileNode *whilePtr;
    IfNode *ifPtr;
    PrintNode *printPtr;
    StatementNode *statementPtr;
};

struct ForNode{
    string num1;
    string num2;
    string identif;
    BlocksNode *blocksPtr;
};

struct WhileNode{
    CondnNode *condnPtr;
    BlocksNode *blocksPtr;
};

struct IfNode{
    CondnNode *condnPtr;
    BlocksNode *blocksTruePtr;
    BlocksNode *blocksFalsePtr;
};

struct PrintNode{
    string STR;
};

struct CondnNode{
    ExprNode *expr1Ptr;
    ExprNode *expr2Ptr;
    string logOp;
};

struct ExprNode{
    string expression;
};


struct StatementNode{
    string ID;
    bool type;
    // TYPE -> FCALL 0, ASSIGNMENT 1
    FCallNode *fCallPtr;
    AssignNode *assignPtr;
};

struct FCallNode{
    vector<string> params;
};

// struct ValueNode{
//     bool type;
//     string val;
// };

struct AssignNode{
    ExprNode *exprPtr;
};

struct FDefNode{
    string fnName;
    vector<string> Args;
    BlocksNode *blocksPtr;
    string retValue;
};

struct BlocksNode{
    vector<BlockNode*> blocks;
};


ProgramNode *parseProgram(int);
ModuleNode *parseX(int);
BlockNode *parseBlock(int);
int parseY();
void parseZ();
void parseA();
PrintNode *parsePrint(int);
WhileNode *parseWhileLoop(int);
ForNode *parseForLoop(int);
IfNode *parseIfElse(int);
BlocksNode *parseBlocks(int);
FDefNode *parseFuncDef(int);
void parseIdList();
string parseExpr();
void parseExpr_();

ProgramNode *parseProgram(int parID){
    NodeInfo[parID] = "MODULE";
    // cout<<"Program\n";
    ProgramNode *node = new ProgramNode;
    if(ptr==tokens.size()){
        // cout<<"Parsed Program using EMPTY\n";
        return node;
    }
    currNum++;
    int xNodeID = currNum;
    adj[parID].push_back(xNodeID);
    ModuleNode *mod = parseX(xNodeID);
    node->modules.push_back(mod);

    currNum++;
    int pNodeID = currNum;
    adj[parID].push_back(pNodeID);
    ProgramNode *node_rem = parseProgram(pNodeID);
    for(auto p:node_rem->modules){
        node->modules.push_back(p);
    }
    // cout<<"Parsed Program\n";
    return node;
}

ModuleNode *parseX(int parID){
    NodeInfo[parID] = "SUBMODULE";
    // cout<<"X\n";
    ModuleNode *node = new ModuleNode; 
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="DEF"){
        node->type = 1;
        currNum++;
        adj[parID].push_back(currNum);
        node->fDefPtr = parseFuncDef(currNum);
    }
    else{
        node->type = 0;
        currNum++;
        adj[parID].push_back(currNum);
        node->blockPtr = parseBlock(currNum);
    }
    // cout<<"Parsed X\n";
    return node;
}

BlockNode *parseBlock(int parID){
    NodeInfo[parID] = "CODEBLOCK";
    // cout<<"Block\n";
    BlockNode *node = new BlockNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="FOR"){
        node->type = 0;
        currNum++;
        adj[parID].push_back(currNum);
        node->forPtr = parseForLoop(currNum); 
    }
    else if(tokens[ptr]=="IF"){
        node->type = 2;
        currNum++;
        adj[parID].push_back(currNum);
        node->ifPtr = parseIfElse(currNum);
    }
    else if(tokens[ptr]=="WHILE"){
        node->type = 1;
        currNum++;
        adj[parID].push_back(currNum);
        node->whilePtr = parseWhileLoop(currNum);
    }
    else if(tokens[ptr]=="PRINT"){
        node->type = 3;
        currNum++;
        adj[parID].push_back(currNum);
        node->printPtr = parsePrint(currNum);
    }
    else{
        node->type = 4;
        currNum++;
        int sNodeID = currNum;
        NodeInfo[sNodeID] = "STATEMENT";
        adj[parID].push_back(currNum);
        StatementNode *snode = new StatementNode;
        node->statementPtr = snode;
        if(ptr>=tokens.size()){
            error("Unexpected EOF");
        }
        if(tokens[ptr]!="ID"){
            error("Unexpected Token! Found "+lexemes[ptr]+", Expected Identifier");
        }
        else{
            snode->ID = lexemes[ptr];
            ptr++;
            int prevptr = ptr;
            int v = parseY();
            int currptr = ptr;
            if(v==0){
                //FUNCTION CALL
                currNum++;
                ll fNodeID = currNum;
                adj[sNodeID].push_back(currNum);
                NodeInfo[fNodeID] = "FUNCTION CALL\\nPARAMS: ";
                FCallNode *fnode = new FCallNode;
                prevptr++;
                currptr--;
                while(prevptr<currptr){
                    fnode->params.push_back(lexemes[prevptr]);
                    NodeInfo[fNodeID] = NodeInfo[fNodeID] + lexemes[prevptr]+" ";
                    prevptr+=2;
                }
                snode->fCallPtr = fnode;
            }
            else{
                //ASSIGNMENT
                currNum++;
                int aNodeID = currNum;
                NodeInfo[aNodeID] = "ASSIGNMENT";
                AssignNode *anode = new AssignNode;
                currNum++;
                int eNodeID = currNum;
                NodeInfo[eNodeID] = "EXPRESSION";
                ExprNode *enode = new ExprNode;
                adj[sNodeID].push_back(aNodeID);
                adj[aNodeID].push_back(eNodeID);
                anode->exprPtr = enode;
                snode->assignPtr = anode;
                prevptr++;
                string expression = "";
                while(prevptr<currptr){
                    expression = expression + lexemes[prevptr];
                    prevptr++;
                }
                enode->expression = expression;
                NodeInfo[eNodeID] = "EXPRESSION\\n"+enode->expression;
            }
            NodeInfo[sNodeID] = "STATEMENT\\nIdentifier: "+snode->ID;
        }
    }
    // cout<<"Parsed Block\n";
    return node;
}

int parseY(){
    // cout<<"Y\n";
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="LPAREN"){
        ptr++;
        parseZ();
        if(ptr>=tokens.size()){
            error("Unexpected EOF");
        }
        if(tokens[ptr]!="RPAREN"){
            error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
        }
        else{
            ptr++;
            // cout<<"Parsed Y using FCALL\n";
            return 0;
        }
    }
    else if(tokens[ptr]=="EQUAL"){
        ptr++;
        parseExpr();
        // cout<<"Parsed Y using Assignment\n";
        return 1;
    }
    else{
        error("Unexpected Token! Found "+lexemes[ptr]);
    }
    return -1;
}

void parseZ(){
    // cout<<"Z\n";
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RPAREN"){
        // cout<<"Parsed Z using EMPTY\n";
        return;
    }
    parseA();
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RPAREN"){
        // cout<<"Parsed Z using Empty*\n";
        return;
    }
    if(tokens[ptr]!="COMMA"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected ,");
    }
    else{
        ptr++;
        parseZ();
        return;
    }
}

void parseA(){
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="NUM"){
        ptr++;
        return;
    }
    else if(tokens[ptr]=="ID"){
        ptr++;
        return;
    }
    else{
        error("Unexpected Token! Found "+lexemes[ptr]);
    }
}

PrintNode *parsePrint(int parID){
    NodeInfo[parID] = "PRINT";
    // cout<<"Print\n";
    PrintNode *node = new PrintNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="PRINT"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected print");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected (");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="STRING"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected String");
    }
    node->STR = lexemes[ptr];
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
    }
    ptr++;
    // cout<<"PARSED PRINT\n";
    NodeInfo[parID] = "PRINT\\n"+node->STR;
    return node;
}

WhileNode *parseWhileLoop(int parID){
    NodeInfo[parID] = "WHILE";
    // cout<<"While\n";
    WhileNode *node = new WhileNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="WHILE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected while");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected (");
    }
    ptr++;
    currNum++;
    int cNodeID = currNum;
    adj[parID].push_back(cNodeID);
    NodeInfo[cNodeID] = "CONDITION";
    CondnNode *cnode = new CondnNode;
    node->condnPtr = cnode;
    currNum++;
    int e1NodeID = currNum;
    adj[cNodeID].push_back(e1NodeID);
    NodeInfo[e1NodeID] = "EXPRESSION";
    ExprNode *exp1 = new ExprNode;
    currNum++;
    int e2NodeID = currNum;
    adj[cNodeID].push_back(e2NodeID);
    NodeInfo[e2NodeID] = "EXPRESSION";
    ExprNode *exp2 = new ExprNode;
    cnode->expr1Ptr = exp1;
    cnode->expr2Ptr = exp2;
    exp1->expression = parseExpr();
    NodeInfo[e1NodeID] = "EXPRESSION\\n"+exp1->expression;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LOGOP"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected a Logical Operator");
    }
    cnode->logOp = lexemes[ptr];
    NodeInfo[cNodeID] = "CONDITION\\nOPERATOR: "+cnode->logOp;
    ptr++;
    exp2->expression = parseExpr();
    NodeInfo[e2NodeID] = "EXPRESSION\\n"+exp2->expression;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected {");
    }
    ptr++;
    currNum++;
    int bNodeID = currNum;
    adj[parID].push_back(bNodeID);
    node->blocksPtr = parseBlocks(bNodeID);
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected }");
    }
    ptr++;
    // cout<<"Parsed While\n";
    return node;
}

ForNode *parseForLoop(int parID){
    NodeInfo[parID] = "FOR\n";
    ForNode *node = new ForNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="FOR"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected for");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="ID"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected Identifier");
    }
    node->identif = lexemes[ptr];
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="IN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected in");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RANGE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected range");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected (");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="NUM"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected a Number");
    }
    node->num1 = lexemes[ptr];
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="COMMA"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected ,");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="NUM"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected a Number");
    }
    node->num2 = lexemes[ptr];
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected {");
    }
    ptr++;
    currNum++;
    int bNodeID = currNum;
    adj[parID].push_back(bNodeID);
    node->blocksPtr = parseBlocks(bNodeID);
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected }");
    }
    ptr++;
    NodeInfo[parID] = "FOR\\n IDENTIFIER: "+node->identif +"\\n Start: "+node->num1+"\\nEnd: "+node->num2;
    return node;
}

IfNode *parseIfElse(int parID){
    NodeInfo[parID] = "IF-ELSE";
    IfNode *node = new IfNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="IF"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected if");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected (");
    }
    ptr++;

    currNum++;
    int cNodeID = currNum;
    NodeInfo[cNodeID] = "CONDITION";
    adj[parID].push_back(cNodeID);
    CondnNode *cnode = new CondnNode;
    node->condnPtr = cnode;
    
    currNum++;
    int e1NodeID = currNum;
    NodeInfo[e1NodeID] = "EXPRESSION";
    adj[cNodeID].push_back(e1NodeID);
    ExprNode *exp1 = new ExprNode;

    currNum++;
    int e2NodeID = currNum;
    NodeInfo[e2NodeID] = "EXPRESSION";
    adj[cNodeID].push_back(e2NodeID);
    ExprNode *exp2 = new ExprNode;
    cnode->expr1Ptr = exp1;
    cnode->expr2Ptr = exp2;
    exp1->expression = parseExpr();
    NodeInfo[e1NodeID] = "EXPRESSION\\n"+exp1->expression;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LOGOP"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected a Logical Operator");
    }
    cnode->logOp = lexemes[ptr];
    NodeInfo[cNodeID] = "CONDITION\\nOPERATOR: "+cnode->logOp;
    ptr++;
    exp2->expression = parseExpr();
    NodeInfo[e2NodeID] = "EXPRESSION\\n"+exp2->expression;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected {");
    }
    ptr++;
    currNum++;
    int bNodeID = currNum;
    adj[parID].push_back(bNodeID);
    node->blocksTruePtr = parseBlocks(bNodeID);
    NodeInfo[bNodeID] = "IF BLOCK";
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected }");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="ELSE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected else");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected {");
    }
    ptr++;
    currNum++;
    int bElseNodeID = currNum;
    adj[parID].push_back(bElseNodeID);
    node->blocksFalsePtr = parseBlocks(bElseNodeID);
    NodeInfo[bElseNodeID] = "ELSE BLOCK";
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected }");
    }
    ptr++;
    return node;
}

BlocksNode *parseBlocks(int parID){
    NodeInfo[parID] = "CODEBLOCKS";
    // cout<<"Blocks\n";
    BlocksNode *node = new BlocksNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RBRACE" || tokens[ptr]=="RETURN"){
        // cout<<"Parsed Blocks using EMPTY\n";
        return node;
    }
    currNum++;
    int bNodeID = currNum;
    adj[parID].push_back(bNodeID);
    node->blocks.push_back(parseBlock(bNodeID));

    currNum++;
    int remBNodeID = currNum;
    adj[parID].push_back(remBNodeID);
    BlocksNode *rem_blocks = parseBlocks(remBNodeID);
    for(auto p:rem_blocks->blocks){
        node->blocks.push_back(p);
    }
    // cout<<"Parsed Blocks\n";
    return node;
}

FDefNode *parseFuncDef(int parID){
    NodeInfo[parID] = "FUNC_DEF";
    FDefNode *node = new FDefNode;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="DEF"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected def");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="ID"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected Identifier");
    }
    node->fnName = lexemes[ptr];
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected (");
    }
    ptr++;
    int prevptr = ptr;
    parseIdList();
    int currptr = ptr;
    while(prevptr<currptr){
        node->Args.push_back(lexemes[prevptr]);
        prevptr+=2;
    }
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="RPAREN"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected )");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]!="LBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected {");
    }
    ptr++;

    currNum++;
    int bNodeID = currNum;
    adj[parID].push_back(bNodeID);
    node->blocksPtr = parseBlocks(bNodeID);
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RETURN"){
        ptr++;
        if(ptr>=tokens.size()){
            error("Unexpected EOF");
        }
        string retvalue = parseExpr();
        node->retValue = retvalue;
    }
    if(tokens[ptr]!="RBRACE"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected }");
    }
    ptr++;
    NodeInfo[parID] = "FUNCTION DEF\\nNAME: "+node->fnName+"\\nARGS: ";
    for(auto arg:node->Args){
        NodeInfo[parID] = NodeInfo[parID] + arg + " ";
    }
    if(node->retValue==""){
        NodeInfo[parID] = NodeInfo[parID] + "\\nRETURNS: VOID";
    }
    else{
        NodeInfo[parID] = NodeInfo[parID] + "\\nRETURNS: "+node->retValue;
    }
    return node;
}

void parseIdList(){
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RPAREN"){
        return;
    }
    if(tokens[ptr]!="ID"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected an Identifier");
    }
    ptr++;
    if(ptr>=tokens.size()){
        error("Unexpected EOF");
    }
    if(tokens[ptr]=="RPAREN"){
        return;
    }
    if(tokens[ptr]!="COMMA"){
        error("Unexpected Token! Found "+lexemes[ptr]+", Expected ,");
    }
    ptr++;
    parseIdList();
}

string parseExpr(){
    string ex = "";
    int prevptr = ptr;
    parseA();
    parseExpr_();
    int currptr = ptr;
    while(prevptr<currptr){
        ex = ex + lexemes[prevptr];
        prevptr++;
    }
    return ex;
}

void parseExpr_(){
    if(tokens[ptr]!="BINOP"){
        return;
    }
    ptr++;
    parseExpr();
}

int main(){
    fstream file;
    file.open("LexerOutput.txt");
    string s;
    while(getline(file,s)){
        bool flag = 0;
        string tok = "";
        string lexe = "";
        for(auto c:s){
            if(flag){
                tok.push_back(c);
            }
            else{
                if(c==' '){
                    flag = 1;
                }
                else{
                    lexe.push_back(c);
                }
            }
        }
        tokens.push_back(tok);
        lexemes.push_back(lexe);
    }

    ProgramNode *root = parseProgram(currNum);
    cout<<"PARSED SUCCESSFULLY\n";
    fstream opdotfile;
    opdotfile.open("ParseTree.dot", fstream::out);
    opdotfile<<"digraph{"<<"\n";
    opdotfile<<"node[shape = squarer]\n";
    for(auto p:NodeInfo){
        opdotfile<<p.first<<"[label=\""<<p.second<<"\"]\n";
    }
    for(auto p:adj){
        for(auto q:adj[p.first]){
            opdotfile<<p.first<<" -> "<<q<<"\n";
        }
    }
    opdotfile<<"}";
    opdotfile.close();
}