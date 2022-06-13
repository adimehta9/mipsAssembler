#include "scanner.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

/*
 * C++ Starter code for CS241 A3
 * All code requires C++14, so if you're getting compile errors make sure to
 * use -std=c++14.
 *
 * This file contains the main function of your program. By default, it just
 * prints the scanned list of tokens back to standard output.
 */
int main() {
  std::string line;
  int pc = -4;
  map<string, int> symbol_table;
  vector<string> ops;
  try {
    while (getline(cin, line)) {
      if (line.size() == 0 || line[0] == ';') {
        continue;
      }
      pc += 4;
      std::vector<Token> tokenLine = scan(line);
      bool prev_label = false;
      int tokL_size = tokenLine.size();
      for (int i = 0; i < tokL_size; i++) {
        if (tokenLine[i].getKind() == Token::LABEL) {
          prev_label = true;
          int size = tokenLine[i].getLexeme().size();

          if (symbol_table.count(tokenLine[i].getLexeme().substr(0, size - 1)) >
              0) {
            throw runtime_error("ERROR: Label already Exists");
          }

          symbol_table.emplace(tokenLine[i].getLexeme().substr(0, size - 1),
                               pc);
          line = line.substr(size, line.size() - size);
          if (line[0] == ' ') {
            line = line.substr(1, line.size() - 1);
          }
          if (i == tokL_size - 1) {
            pc -= 4;
          }
        } else if (prev_label == true) {
          ops.push_back(line);
          break;
        }
      }

      if (prev_label == false) {
        ops.push_back(line);
      }
    }

    // Print out vectors
    /*  for (std::vector<string>::iterator it = ops.begin(); it != ops.end();
          ++it) {
       string line = *it;
       cout << line << endl;
     }

     cout << "Vectors ended" << endl;
    */
    pc = -4;

    for (std::vector<string>::iterator it = ops.begin(); it != ops.end();
         ++it) {
      pc += 4;

      string line = *it;
      std::vector<Token> tokenLine = scan(line);

      // .word
      if (tokenLine[0].getKind() == Token::WORD) {
        if (tokenLine.size() != 2) {
          throw runtime_error("ERROR: Incorrect size");
        } else if (tokenLine[1].getKind() != Token::INT &&
                   tokenLine[1].getKind() != Token::HEXINT &&
                   tokenLine[1].getKind() != Token::ID) {
          throw runtime_error("ERROR: .word requires int, hexint or label");
        }
        // .word label
        if (tokenLine[1].getKind() == Token::ID) {
          if (symbol_table.count(tokenLine[1].getLexeme()) <= 0) {
            throw runtime_error("ERROR: Label doesn't exist");
          }
          int num = symbol_table[tokenLine[1].getLexeme()];
          unsigned char c = num >> 24;
          cout << c;
          c = num >> 16;
          cout << c;
          c = num >> 8;
          cout << c;
          c = num;
          cout << c;
        } else if (tokenLine[1].getKind() == Token::INT) { // .word 5
          int64_t num = tokenLine.at(1).toNumber();
          if (num < -2147483648 || num > 4294967295) {
            throw runtime_error("ERROR: does not fit within range");
          }
          unsigned char c = num >> 24;
          cout << c;
          c = num >> 16;
          cout << c;
          c = num >> 8;
          cout << c;
          c = num;
          cout << c;
        } else if (tokenLine[1].getKind() == Token::HEXINT) { // .word 0xf
          int64_t num = tokenLine.at(1).toNumber();
          if (num > 4294967295) {
            throw runtime_error("ERROR: does not fit within range");
          }
          unsigned char c = num >> 24;
          cout << c;
          c = num >> 16;
          cout << c;
          c = num >> 8;
          cout << c;
          c = num;
          cout << c;
        }

      } else if (tokenLine[0].getLexeme() == "add" || // add, sub, slt, sltu
                 tokenLine[0].getLexeme() == "sub" ||
                 tokenLine[0].getLexeme() == "slt" ||
                 tokenLine[0].getLexeme() == "sltu") {
        if (tokenLine.size() != 6) {
          throw runtime_error("ERROR: Expected 3 Registers and 2 Commas");
        } else if (tokenLine[1].getKind() != Token::REG ||
                   tokenLine[2].getKind() != Token::COMMA ||
                   tokenLine[3].getKind() != Token::REG ||
                   tokenLine[4].getKind() != Token::COMMA ||
                   tokenLine[5].getKind() != Token::REG) {
          throw runtime_error(
              "ERROR: Opcode add requires 3 registers separated by commas");
        }

        int64_t d = tokenLine[1].toNumber();
        int64_t s = tokenLine[3].toNumber();
        int64_t t = tokenLine[5].toNumber();

        if (d < 0 || d > 31 || s < 0 || s > 31 || t < 0 || t > 31) {
          throw runtime_error("ERROR: Out of Range");
        }
        int instr;
        if (tokenLine[0].getLexeme() == "add") {
          instr = (0 << 26) | (s << 21) | (t << 16) | (d << 11) | 32;
        } else if (tokenLine[0].getLexeme() == "sub") {
          instr = (0 << 26) | (s << 21) | (t << 16) | (d << 11) | 34;
        } else if (tokenLine[0].getLexeme() == "slt") {
          instr = (0 << 26) | (s << 21) | (t << 16) | (d << 11) | 42;
        } else if (tokenLine[0].getLexeme() == "sltu") {
          instr = (0 << 26) | (s << 21) | (t << 16) | (d << 11) | 43;
        }
        unsigned char c = instr >> 24;
        cout << c;
        c = instr >> 16;
        cout << c;
        c = instr >> 8;
        cout << c;
        c = instr;
        cout << c;

      } else if (tokenLine[0].getLexeme() == "mult" || // mult, multu, div, divu
                 tokenLine[0].getLexeme() == "multu" ||
                 tokenLine[0].getLexeme() == "div" ||
                 tokenLine[0].getLexeme() == "divu") {
        if (tokenLine.size() != 4) {
          throw runtime_error("ERROR: Expected 2 registers with comma");
        } else if (tokenLine[1].getKind() != Token::REG ||
                   tokenLine[2].getKind() != Token::COMMA ||
                   tokenLine[3].getKind() != Token::REG) {
          throw runtime_error("ERROR: Not in correct order");
        }
        int64_t s = tokenLine[1].toNumber();
        int64_t t = tokenLine[3].toNumber();

        if (s < 0 || s > 31 || t < 0 || t > 31) {
          throw runtime_error("ERROR: Out of Range");
        }

        int instr;
        if (tokenLine[0].getLexeme() == "mult") {
          instr = (0 << 26) | (s << 21) | (t << 16) | 24;
        } else if (tokenLine[0].getLexeme() == "multu") {
          instr = (0 << 26) | (s << 21) | (t << 16) | 25;
        } else if (tokenLine[0].getLexeme() == "div") {
          instr = (0 << 26) | (s << 21) | (t << 16) | 26;
        } else if (tokenLine[0].getLexeme() == "divu") {
          instr = (0 << 26) | (s << 21) | (t << 16) | 27;
        }
        unsigned char c = instr >> 24;
        cout << c;
        c = instr >> 16;
        cout << c;
        c = instr >> 8;
        cout << c;
        c = instr;
        cout << c;

      } else if (tokenLine[0].getLexeme() == "mfhi" || // mfhi, mflo, lis
                 tokenLine[0].getLexeme() == "mflo" ||
                 tokenLine[0].getLexeme() == "lis") {
        if (tokenLine.size() != 2) {
          throw runtime_error("ERROR: Expected One Register");
        } else if (tokenLine[1].getKind() != Token::REG) {
          throw runtime_error("ERROR: only needs one register");
        }
        int64_t d = tokenLine[1].toNumber();
        if (d < 0 || d > 31) {
          throw runtime_error("ERROR: Out of Range");
        }
        int instr;
        if (tokenLine[0].getLexeme() == "mfhi") {
          instr = (0 << 16) | (d << 11) | 16;
        } else if (tokenLine[0].getLexeme() == "mflo") {
          instr = (0 << 16) | (d << 11) | 18;
        } else if (tokenLine[0].getLexeme() == "lis") {
          instr = (0 << 16) | (d << 11) | 20;
        }
        unsigned char c = instr >> 24;
        cout << c;
        c = instr >> 16;
        cout << c;
        c = instr >> 8;
        cout << c;
        c = instr;
        cout << c;

      } else if (tokenLine[0].getLexeme() == "beq" || // beq, bne
                 tokenLine[0].getLexeme() == "bne") {
        if (tokenLine.size() != 6) {
          throw runtime_error("ERROR: Expected 2 registers and offset");
        } else if (tokenLine[1].getKind() != Token::REG ||
                   tokenLine[2].getKind() != Token::COMMA ||
                   tokenLine[3].getKind() != Token::REG ||
                   tokenLine[4].getKind() != Token::COMMA ||
                   (tokenLine[5].getKind() != Token::INT &&
                    tokenLine[5].getKind() != Token::HEXINT &&
                    tokenLine[5].getKind() != Token::ID)) {
          throw runtime_error("ERROR: Requires form beq $d, $s, offset");
        }
        int64_t s = tokenLine[1].toNumber();
        int64_t t = tokenLine[3].toNumber();

        if (s < 0 || s > 31 || t < 0 || t > 31) {
          throw runtime_error("ERROR: Out of Range");
        }

        if (tokenLine[5].getKind() == Token::ID) { // if beq $3, $3, label
          // check if ID is in symbol_table
          if (symbol_table.count(tokenLine[5].getLexeme()) <= 0) {
            throw runtime_error("ERROR: Label doesn't exist");
          }
          int offset = (symbol_table[tokenLine[5].getLexeme()] - (4 + pc)) / 4;

          if (offset < -32768 || offset > 32767) {
            throw runtime_error("ERROR: Offset not within range");
          }

          int instr;
          if (tokenLine[0].getLexeme() == "beq") {
            instr = (4 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          } else if (tokenLine[0].getLexeme() == "bne") {
            instr = (5 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          }
          unsigned char c = instr >> 24;
          cout << c;
          c = instr >> 16;
          cout << c;
          c = instr >> 8;
          cout << c;
          c = instr;
          cout << c;
        } else if (tokenLine[5].getKind() == Token::INT) { // beq $3, $5, 3
          int64_t offset = tokenLine[5].toNumber();
          if (offset < -32768 || offset > 32767) {
            throw runtime_error("ERROR: Offset not within range");
          }
          int instr;
          if (tokenLine[0].getLexeme() == "beq") {
            instr = (4 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          } else if (tokenLine[0].getLexeme() == "bne") {
            instr = (5 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          }
          unsigned char c = instr >> 24;
          cout << c;
          c = instr >> 16;
          cout << c;
          c = instr >> 8;
          cout << c;
          c = instr;
          cout << c;
        } else if (tokenLine[5].getKind() == Token::HEXINT) { // beq $4, $3, 0xf
          int64_t offset = tokenLine[5].toNumber();
          if (offset > 65535) {
            throw runtime_error("ERROR: Offset not within range");
          }
          int instr;
          if(tokenLine[0].getLexeme() == "beq"){
            instr = (4 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          } else if(tokenLine[0].getLexeme() == "bne"){
            instr = (5 << 26) | (s << 21) | (t << 16) | (offset & 0xFFFF);
          }
          unsigned char c = instr >> 24;
          cout << c;
          c = instr >> 16;
          cout << c;
          c = instr >> 8;
          cout << c;
          c = instr;
          cout << c;
        }
      } else if (tokenLine[0].getLexeme() == "lw" ||
                 tokenLine[0].getLexeme() == "sw") {
        if (tokenLine.size() != 7) {
          throw runtime_error("ERROR: Incorrect number of Parameters");
        } else if (tokenLine[1].getKind() != Token::REG ||
                   tokenLine[2].getKind() != Token::COMMA ||
                   (tokenLine[3].getKind() != Token::INT &&
                    tokenLine[3].getKind() != Token::HEXINT) ||
                   tokenLine[4].getKind() != Token::LPAREN ||
                   tokenLine[5].getKind() != Token::REG ||
                   tokenLine[6].getKind() != Token::RPAREN) {
          throw runtime_error("ERROR: Incorrect Token Type");
        }
        int64_t t = tokenLine[1].toNumber();
        int64_t i = tokenLine[3].toNumber();
        int64_t s = tokenLine[5].toNumber();

        if (tokenLine[3].getKind() == Token::INT) {
          if (i > 32767 || i < -32768) {
            throw runtime_error("ERROR: Out of Range");
          }
        } else if (tokenLine[3].getKind() == Token::HEXINT) {
          if (i > 0xffff) {
            throw runtime_error("ERROR: Out of Range");
          }
        }

        int instr;
        if (tokenLine[0].getLexeme() == "lw") {
          instr = (35 << 26) | (s << 21) | (t << 16) | (i & 0xFFFF);
        } else if (tokenLine[0].getLexeme() == "sw") {
          instr = (43 << 26) | (s << 21) | (t << 16) | (i & 0xFFFF);
        }

        unsigned char c = instr >> 24;
        cout << c;
        c = instr >> 16;
        cout << c;
        c = instr >> 8;
        cout << c;
        c = instr;
        cout << c;

      } else if (tokenLine[0].getLexeme() == "jr" ||
                 tokenLine[0].getLexeme() == "jalr") {
        if (tokenLine.size() != 2) {
          throw runtime_error("ERROR: Incorrect number of parameters");
        } else if (tokenLine[1].getKind() != Token::REG) {
          throw runtime_error("ERROR: Wrong toke type");
        }

        int64_t s = tokenLine[1].toNumber();

        if(s < 0 || s > 31){
          throw runtime_error("ERROR: Out of Range");
        }
        int instr;
        if(tokenLine[0].getLexeme() == "jr"){
          instr = (0 << 26) | (s << 21) | 8;
        } else if(tokenLine[0].getLexeme() == "jalr"){
          instr = (0 << 26) | (s << 21) | 9;
        }

        unsigned char c = instr >> 24;
        cout << c;
        c = instr >> 16;
        cout << c;
        c = instr >> 8;
        cout << c;
        c = instr;
        cout << c;
        
      } else {
        throw runtime_error("ERROR: Invalid Opcode");
      }
    }

  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  } catch (runtime_error &error) {
    cerr << error.what() << endl;
  }

  return 0;
}
