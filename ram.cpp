//TODO: Use 0 index for memory in assembly file.
//		Implement multidigit numbers in memory files.

// Implementation of RAM class
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "ram.h"
#include "inst.h"
using namespace std;

std::string opCodeToString(OPCODES code);


RAM::RAM()
{
    memory = vector<int>();
    program = vector<Instruction>();
    pc = 1;  ac = 0;
    /*for (int i=0;i<=memorySize;i++){
     memory[i] = 0;
     }*/
}

// Initialize RAM with hardwired program and memory
// pc is set to 1 and ac is set to 0.
void RAM::init()
{
    program[1].opcode = LDA;
    program[1].operand = 3;
    program[2].opcode = SUB;
    program[2].operand = 4;
    program[3].opcode = JMZ;
    program[3].operand = 7;
    program[4].opcode = LDA;
    program[4].operand = 1;
    program[5].opcode = STA;
    program[5].operand = 5;
    program[6].opcode = HLT;
    program[7].opcode = LDA;
    program[7].operand = 2;
    program[8].opcode = STA;
    program[8].operand = 5;
    program[9].opcode = HLT;
    
    memory[1] = 0;
    memory[2] = 1;
    memory[3] = 2;
    memory[4] = 1;
    memory[5] = 3;
    pc = 1;  ac = 0;
}

// Initialize RAM with program in file with the name pInput
// and initial memory configuration in the file with name mInput
// pc is set to 1 and ac is set to 0.  programSize is set to the number
// of instructions read.
void RAM::init(const char *file) {
    // Initialize Memory
    const int MAXSTRSIZE = 160;
    string str;
    int addr = 1;
    
    
    // Initialize memory
    ifstream mFile(file);
    if (!mFile) {
        cerr << "Error: program file not found" << endl;
        exit(1);
    }
    
    // Initialize program
    string instName;
    bool inMemory = false;
    pc = 0;
    int line = 0;
    cout << "reading file: " << file << "\n";
    while (mFile >> instName) {
        if (!inMemory) {
            program.push_back(*((Instruction*)malloc(sizeof(Instruction))));
        }
        //instName = str;
        cout << "\nin\n";
        line++;
        //instName = str;
        if (inMemory) {
            memory.push_back(instName[0]-'0');
            getline(mFile, str, '\n');  // flush line (possibly contains comment)
            addr++;
        }else if (instName[0] == ';') {
            getline(mFile, str, '\n'); } // flush to end of line
        else if (instName == "&") {
            inMemory = true; }
        else if (instName == "LDA") {
            program[pc].opcode = LDA;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "LDI") {
            program[pc].opcode = LDI;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "STA") {
            program[pc].opcode = STA;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "STI") {
            program[pc].opcode = STI;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "ADD") {
            program[pc].opcode = ADD;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "SUB") {
            program[pc].opcode = SUB;
            mFile >> program[pc].operand; 
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "JMP") {
            program[pc].opcode = JMP;
            mFile >> program[pc].operand; 
            program[pc].operand--;
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "JMZ") {
            program[pc].opcode = JMZ;
            mFile >> program[pc].operand; 
            program[pc].operand--;
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "JMN") {
            program[pc].opcode = JMN;
            mFile >> program[pc].operand; 
            program[pc].operand--;
            getline(mFile, str, '\n');  pc++; }
        else if (instName == "HLT") {
            program[pc].opcode = HLT;
            getline(mFile, str, '\n');  pc++; }
        else { cerr << "Error:  Illegal Instruction " << instName << " one line " << line << "\n";
            exit(1); }
    }
    pc = 0;
    ac = 0;
}

// simulate execution of RAM with given program and memory configuration.
// Notes:
//    1. Program may not terminate (if HLT is not executed)
//    2. Currently no error checking is performed.  Checks for valid program
//       and memory addresses and illegal opcodes should be provided.
void RAM::execute() {
    int x;
    OPCODES op;
    int size = program.size();
    while (pc < size) {
        op = program[pc].opcode;
        cout << pc << "\t:\t" << opCodeToString(op) << " " << program[pc].operand << "\n";
        switch (op) {
            case LDA:
                x = program[pc].operand;
                ac = memory[x];
                pc++;
                break;
                
            case LDI:
                x = program[pc].operand;
                ac = memory[memory[x]];
                pc++;
                break;
                
            case STA:
                x = program[pc].operand;
                memory[x] = ac;
                pc++;
                break;
                
            case STI:
                x = program[pc].operand;
                memory[memory[x]] = ac;
                pc++;
                break;
                
            case ADD:
                x = program[pc].operand;
                ac = ac + memory[x];
                pc++;
                break;
                
            case SUB:
                x = program[pc].operand;
                ac = ac - memory[x];
                pc++;
                break;
                
            case JMP:
                x = program[pc].operand;
                pc = x;
                break;
                
            case JMZ:
                x = program[pc].operand;
                if (ac == 0)
                    pc = x;
                else
                    pc++;
                break;
            case JMN:
                x = program[pc].operand;
                if (ac < 0)
                    pc = x;
                else
                    pc++;
                break;
            case HLT:
                pc = size;
                break;
        }
    }
}

RAM* RAM::fork() {
    return new RAM(*this);
}

string RAM::toString() {
    ostringstream oss;
    for (int i=0; i<program.size(); i++) {
        oss << program[i].opcode << " " << program[i].operand << ";\n";
    }
    oss << "&\n";
    for (int i=0; i<memory.size(); i++) {
        oss << memory[i] << "\n";
    }
    return oss.str();
}

// Dump memory contents

void RAM::dump()
{
    cout << "RAM Memory Contents" << endl;
    cout << endl;
    for (int i=1;i<=memory.size();i++)
        cout << i << "   " << memory[i] << endl;
}

std::string opCodeToString(OPCODES code) {
    switch (code) {
        case LDA:
            return "LDA";
            break;
        case LDI:
            return "LDI";
            break;
        case STA:
            return "STA";
            break;
        case STI:
            return "STI";
            break;
        case ADD:
            return "ADD";
            break;
        case SUB:
            return "SUB";
            break;
        case JMP:
            return "JMP";
            break;
        case JMN:
            return "JMN";
            break;
        case HLT:
            return "HLT";
            break;
        default:
            return "NOP";
    }
}


