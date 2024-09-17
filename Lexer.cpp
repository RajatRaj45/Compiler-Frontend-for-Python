#include <bits/stdc++.h>
using namespace std;

vector<string> st_lexemes;

class DFA{
    map<pair<int,char>,int> transitions;
    set<int> final_states;

    public:
        
        DFA(){
            final_states.clear();
        }
        
        DFA(string s){
            for(int i=0;i<(s.size());i++){
                transitions[{i,s[i]}] = i+1;
            }
            final_states.insert(s.size());
        }
        
        void DFA_id(){
            for(int i=0;i<26;i++){
                char ch = ('a' + i);
                char chc = ('A' + i);
                transitions[{0,ch}] = 1;
                transitions[{0,chc}] = 1;
                transitions[{1,chc}] = 1;
                transitions[{1,ch}] = 1;
            }
            for(int i=0;i<=9;i++){
                char ch = ('0'+i);
                transitions[{1,ch}] = 1;
            }
            transitions[{1,'_'}] = 1;
            final_states.insert(1);
        }

        void DFA_num(){
            for(int i=1;i<=9;i++){
                char ch = ('0'+i);
                transitions[{0,ch}] = 1;
            }
            for(int i=0;i<=9;i++){
                char ch = ('0'+i);
                transitions[{1,ch}] = 1;
            }
            transitions[{0,'0'}] = 2;
            final_states.insert(1);
            final_states.insert(2);
        }

        void DFA_binop(){
            transitions[{0,'+'}] = 1;
            // transitions[{0,'='}] = 1;
            transitions[{0,'-'}] = 1;
            transitions[{0,'*'}] = 1;
            transitions[{0,'/'}] = 1;
            transitions[{0,'%'}] = 1;
            final_states.insert(1);
        }
        
        void DFA_logop(){
            transitions[{0,'<'}] = 1;
            transitions[{0,'>'}] = 1;
            transitions[{1,'='}] = 5;

            transitions[{0,'='}] = 3;
            transitions[{3,'='}] = 2;
            final_states.insert(1);
            final_states.insert(2);
            final_states.insert(5);
        }

        void DFA_string(){
            transitions[{0,(char)(34)}] = 1;
            for(int i=0;i<26;i++){
                transitions[{1,(char)('a'+i)}] = 1;
                transitions[{1,(char)('A'+i)}] = 1;
            }
            transitions[{1,(char)(34)}] = 2;
            final_states.insert(2);
        }

        bool parse(string s){
            int curr_state = 0;
            for(int i=0;i<s.size();i++){
                if(transitions.find({curr_state,s[i]})==transitions.end()){
                    return 0;
                }
                curr_state = transitions[{curr_state,s[i]}];
            }
            if(final_states.find(curr_state)!=final_states.end()){
                return 1;
            }
            return 0;
        }
};

void initialiseTokens(vector<DFA> &tokens){
    DFA da("for");
    tokens.push_back(da);
    DFA db("in");
    tokens.push_back(db);
    DFA dc("range");
    tokens.push_back(dc);
    DFA dd("(");
    tokens.push_back(dd);
    DFA de(")");
    tokens.push_back(de);
    DFA df("{");
    tokens.push_back(df);
    DFA dg("}");
    tokens.push_back(dg);
    DFA dh("if");
    tokens.push_back(dh);
    DFA di("else");
    tokens.push_back(di);
    DFA dj("print");
    tokens.push_back(dj);
    DFA dk(",");
    tokens.push_back(dk);
    DFA dl("STRING");
    tokens.push_back(dl);
    DFA dm("while");
    tokens.push_back(dm);
    DFA dn("=");
    tokens.push_back(dn);
    DFA dp("def");
    tokens.push_back(dp);
    DFA dq("return");
    tokens.push_back(dq);
    
    DFA d1;
    d1.DFA_num();
    tokens.push_back(d1);
    DFA d2;
    d2.DFA_id();
    tokens.push_back(d2);
    DFA d3;
    d3.DFA_binop();
    tokens.push_back(d3);
    DFA d4;
    d4.DFA_logop();
    tokens.push_back(d4);
}

void preprocess(string &code){
    string c1 = "";
    string c2 = "";
    string temp = "";
    bool flag = 0;
    for(auto c:code){
        if(!flag){
            if(c!='\"'){
                c1.push_back(c);
            }
            else{
                flag = 1;
            }
        }
        else{
            if(c=='\"'){
                c1 = c1 + "STRING";
                st_lexemes.push_back(temp);
                temp = "";
                flag = 0;
            }
            else{
                temp.push_back(c);
            }
        }
    }
    if(!flag){
        code = c1;
    }
}

int main(){
    bool error = 0;
    fstream filesrc;
    filesrc.open("srcCode.py");
    vector<string> srcCode;
    string s;
    int prev = 0;
    while(getline(filesrc,s)){
        int count = 0;
        while(s[0]==' '){
            s = s.substr(1);
            count++;
        }
        if((count/4)>prev){
            srcCode[srcCode.size()-1].pop_back();
            srcCode[srcCode.size()-1].push_back('{');
        }
        int c = prev - (count/4);
        while(c>0){
            srcCode.push_back("}");
            c--;
        }
        prev = (count/4);
        srcCode.push_back(s);
    }
    while(prev!=0){
        srcCode.push_back("}");
        prev--;    
    }
    fstream opPreFile;
    opPreFile.open("PreprocessedOutput.txt", fstream::out);
    for(auto s:srcCode){
        opPreFile<<s<<"\n";
    }
    opPreFile.close();


    fstream file;
    file.open("PreprocessedOutput.txt");
    string sourceCode = "";
    s = "";
    while(getline(file,s)){
        sourceCode += s;
        sourceCode += " ";
    }
    // cout<<sourceCode<<"\n\n";
    preprocess(sourceCode);
    // cout<<sourceCode<<"\n\n";
    
    vector<DFA> tokens;
    initialiseTokens(tokens);
    int prevToken = -1;
    string curr = "";
    vector<pair<string,int>> res;
    
    for(int i=0;i<sourceCode.size();i++){
        if(sourceCode[i]==' '){
            if(prevToken==-1){
                cout<<"LEXICAL ERROR!!!\n";
                error = 1;
                break;
            }
            string lexeme = curr;
            res.push_back({lexeme,prevToken});
            curr = "";
            prevToken = -1;
            continue;
        }
        curr.push_back(sourceCode[i]);
        bool matched = 0;
        for(int j=0;j<tokens.size();j++){
            DFA d = tokens[j];
            if(d.parse(curr)){
                prevToken = j;
                matched = 1;
                break;    
            }
        }
        if(!matched){
            if(prevToken==-1){
                cout<<"LEXICAL ERROR!!!\n";
                error = 1;
                break;
            }
            string lexeme = curr;
            lexeme.pop_back();
            res.push_back({lexeme,prevToken});
            curr = "";
            prevToken = -1;
            i--;
        }
        if(i==(sourceCode.size()-1)){
            if(prevToken==-1){
                cout<<"LEXICAL ERROR!!!\n";
                error = 1;
                break;
            }
            else{
                res.push_back({curr,prevToken});
                break;
            }
        }
    }
    
    cout<<"\nTOKENS:\n";
    
    map<int, string> token_name;
    token_name[0] = "FOR";
    token_name[1] = "IN";
    token_name[2] = "RANGE";
    token_name[3] = "LPAREN";
    token_name[4] = "RPAREN";
    token_name[5] = "LBRACE";
    token_name[6] = "RBRACE";
    token_name[7] = "IF";
    token_name[8] = "ELSE";
    token_name[9] = "PRINT";
    token_name[10] = "COMMA";
    token_name[11] = "STRING";
    token_name[12] = "WHILE";
    token_name[13] = "EQUAL";
    token_name[14] = "DEF";
    token_name[15] = "RETURN";
    token_name[16] = "NUM";
    token_name[17] = "ID";
    token_name[18] = "BINOP";
    token_name[19] = "LOGOP";

    for(auto p:res){
        cout<<p.first<<" "<<token_name[p.second]<<"\n";
    }

    if(error){
        cout<<"LEXICAL ERROR ENCOUNTERED!\n";
    }
    else{
        cout<<"TOKENISATION SUCCESSFUL!\n";
    }

    fstream opfile;
    opfile.open("LexerOutput.txt", fstream::out);
    int st_ptr = 0;
    for(auto p:res){
        if(p.first=="STRING"){
            opfile<<st_lexemes[st_ptr]<<" "<<token_name[p.second]<<"\n";
            st_ptr++;
        }
        else{
            opfile<<p.first<<" "<<token_name[p.second]<<"\n";
        }
    }
    opfile.close();
}