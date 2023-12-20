#include <map>
#include <deque>
#include <cctype>
#include <string>
#include <utility>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

enum Token {
  literal, identifier, addition, deduction, multiplication, equals, lparentheses, rparentheses, newline, semicolon, error, eof
};

struct base {
  Token token;
  string word;
};

string translate(Token t) {
  switch(t) {
    case literal: return "literal"; break;
    case identifier: return "identifier"; break;
    case addition: return "+"; break;
    case deduction: return "-"; break;
    case multiplication: return "*"; break;
    case lparentheses: return "("; break;
    case rparentheses: return ")"; break;
    case equals: return "="; break;
    case newline: return "\n"; break;
    case semicolon: return "semicolon"; break;
    case error: return "error\n"; break;
    case eof: return "eof"; break;
  }
  return "error";
}

class Lexer {
  private:
    ifstream inFile;
    pair<Token, string> cur_token;
    pair<Token, string> get_token() {
      char c;
      string word, line;
      while(inFile.get(c)) {
        if(isdigit(c)) {
          if(c == '0') {
            char next = inFile.peek();
            if(isdigit(next)) { 
              getline(inFile, line);
              return make_pair(error, "error"); 
            }
            else return make_pair(literal, "0");
          } else if(c >= '1' && c <= '9') { 
            while(isdigit(c)) {
              word += c;
              inFile.get(c);
            }
            inFile.putback(c);
            return make_pair(literal, word);
          }
        } else if(isalpha(c) || c == '_') {
            while(isalnum(c) || c == '_') {
              word += c;
              inFile.get(c);
            }
          inFile.putback(c);
          return make_pair(identifier, word);
        }
        else if(c == '+') return make_pair(addition, "+");
        else if(c == '-') return make_pair(deduction, "-");
        else if(c == '*') return make_pair(multiplication, "*");
        else if(c == '=') return make_pair(equals, "=");
        else if(c == '(') return make_pair(lparentheses, "(");
        else if(c == ')') return make_pair(rparentheses, ")");
        else if(c == ';') return make_pair(semicolon, ";");
      }
      return make_pair(eof, "End of file");
    }

  public:
    Lexer() = delete;
    Lexer(string filename) {
      inFile.open(filename);
    }
    ~Lexer() {
      inFile.close();
    }
    deque<pair<Token, string>> tokenizer() {
      deque<pair<Token, string>> t;
      cur_token = get_token();
      while(cur_token.first != eof) {
        t.push_back(cur_token);
        cur_token = get_token();
      }
      t.push_back(make_pair(eof, "End of file"));
      return t;
    }
};

class Parser {
  private:
    deque<pair<Token, string>> tokens;
    pair<Token, string> cur_token;
    map<string, int> state;

  public:
    Parser(deque<pair<Token, string>> tokens) {
      this->tokens = tokens;
    }
    void get_token() {
      cur_token = tokens.front();
      tokens.pop_front();
    }
    void match(Token t) {
      if(cur_token.first == t) {
        get_token();
      } else {
        cout << "Error\n";
        exit(-1);
      }
    }
    void program() {
      get_token();
      while(!tokens.empty()) {
        assignment();
      }
      for (auto const& x : state){
        cout << x.first << " = " << x.second << endl ;
      }
    }
    void assignment() {
      if(cur_token.first != identifier) {
        cout << "invalid program\n";
        exit(-1);
      } else {
          string foo = cur_token.second;
         match(identifier);
          match(equals);
          int x = exp();
          state[foo] = x;
         match(semicolon);
      }
    }
    int expPrime() {
      if(cur_token.first == addition) {
        match(addition);
        return exp();
      } else if(cur_token.first == deduction) {
        match(deduction);
        return -exp();
      }
      return 0;
    }
    int exp() {
      int i = term();
      if(cur_token.first == addition || cur_token.first == deduction) {
        i += expPrime();
      }
      return i;
    }
    int term() {
      int i = fact();
      if(cur_token.first == multiplication) {
        i *= termPrime();
      }
      return i;
    }
    int termPrime() {
      match(multiplication);
      return term();
    }
    int fact() {
      if(cur_token.first == lparentheses) {
        match(lparentheses);
        int i = exp();
        match(rparentheses);
        return i;
      } else if(cur_token.first == deduction) {
          match(deduction);
          return -fact();
      } else if(cur_token.first == addition) {
          match(addition);
          return fact();
      } else if(cur_token.first == literal) {
          int i = stoi(cur_token.second);
          match(literal);
          return i;
      } else if(cur_token.first == identifier) {
          if(state.find(cur_token.second) == state.end()) { 
            match(semicolon);
            cout<< "Error/n";
            exit(-1);
          }
        string tmp = cur_token.second;
        match(identifier);
        return state[tmp];
      }
      return 0;
    }
};

int main() {
  string filename = "input4.txt";
  Lexer test(filename);
  deque<pair<Token, string>> t = test.tokenizer();
  Parser test1(t);
  test1.program();
  
  return 0;
}
