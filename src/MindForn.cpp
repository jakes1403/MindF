// MindForn : Based on the specs provided in this wikipedia article: 
// https://en.wikipedia.org/wiki/Brainfuck
// Written by Jacob Allen 5/19/2021
//

#include <iostream>
#include <string>
#include "TFile.h"

using namespace std;

// Number of Cells to use in the BrainF virtual machine:
const size_t MACHINE_SIZE = 30000;

// Convert a string into BrainF code that prints the given string in the current cell
string textToBrainF(string text)
{
    string code = "";
    for (size_t i = 0; i < text.size(); i++)
    {
        char letter = text[i];
        if (i == 0)
        {
            for (char b = 0; b < letter; b++)
            {
                code += '+';
            }
            code += '.';
        }
        else
        {
            char last = text[i - 1];
            if (letter > last)
            {
                for (char b = 0; b < letter - last; b++)
                {
                    code += '+';
                }
            }
            else
            {
                for (char b = 0; b < last - letter; b++)
                {
                    code += '-';
                }
            }
            code += '.';
        }
    }
    code += "[-]";
    return code;
}

// Instructions name enum
enum Instructions
{
    INCBY = 1,
    DECBY = 2,
    SETCELL = 3,
    GOTOCELL = 4,
    INCCELLBY = 5,
    DECCELLBY = 6,
    STARTLOOP = 7,
    ENDLOOP = 8,
    WRITE = 9,
    READ = 10
};

// Convert instruction enum to string
string INS_TO_STRING(char s)
{
    switch (s)
    {
    case INCBY:
    {
        return "INCBY";
        break;
    }
    case DECBY:
    {
        return "DECBY";
        break;
    }
    case SETCELL:
    {
        return "SETCELL";
        break;
    }
    case GOTOCELL:
    {
        return "GOTOCELL";
        break;
    }
    case INCCELLBY:
    {
        return "INCCELLBY";
        break;
    }
    case DECCELLBY:
    {
        return "DECCELLBY";
        break;
    }
    case STARTLOOP:
    {
        return "STARTLOOP";
        break;
    }
    case ENDLOOP:
    {
        return "ENDLOOP";
        break;
    }
    case WRITE:
    {
        return "WRITE";
        break;
    }
    case READ:
    {
        return "READ";
        break;
    }
    default:
    {
        return "NOOP";
    }
    }
}

// The instruction format for the optimizer
struct Instruction
{
    char opcode;
    size_t data;
};

// BrainF program optimizer - Optimizes brainf code into vector of instructions
vector<Instruction> OptimizeProgram(string prog)
{
    vector<Instruction> optimizedList;
    string progCleaned = "";
    // First step - Remove all comments, etc and make sure all brackets have a closer
    int checkCounter = 0;
    for (size_t pc = 0; pc < prog.size(); pc++)
    {
        switch (prog[pc])
        {
        case '>':
        {
            progCleaned += '>';
            break;
        }
        case '<':
        {
            progCleaned += '<';
            break;
        }
        case '+':
        {
            progCleaned += '+';
            break;
        }
        case '-':
        {
            progCleaned += '-';
            break;
        }
        case '.':
        {
            progCleaned += '.';
            break;
        }
        case ',':
        {
            progCleaned += ',';
            break;
        }
        case '[':
        {
            progCleaned += '[';
            checkCounter++;
            break;
        }
        case ']':
        {
            progCleaned += ']';
            checkCounter--;
            break;
        }
        }
        if (checkCounter < 0)
        {
            cout << "Brackets uneven!\nCheck to make sure each '[' has a corresponding ']' and vice versa!" << endl;
            exit(-1);
        }
    }
    // Step 2 - Create program and merge repeated operations
    for (size_t i = 0; i < progCleaned.size(); i++)
    {
        Instruction ins;
        switch (progCleaned[i])
        {
        case '>':
        {
            size_t counter = 0;
            for (; progCleaned[i++] == '>'; counter++);
            ins.opcode = INCCELLBY;
            ins.data = counter;
            i -= 2;
            break;
        }
        case '<':
        {
            size_t counter = 0;
            for (; progCleaned[i++] == '<'; counter++);
            ins.opcode = DECCELLBY;
            ins.data = counter;
            i -= 2;
            break;
        }
        case '+':
        {
            size_t counter = 0;
            for (; progCleaned[i++] == '+'; counter++);
            ins.opcode = INCBY;
            ins.data = counter;
            i -= 2;
            break;
        }
        case '-':
        {
            size_t counter = 0;
            for (; progCleaned[i++] == '-'; counter++);
            ins.opcode = DECBY;
            ins.data = counter;
            i -= 2;
            break;
        }
        case '.':
        {
            ins.opcode = WRITE;
            break;
        }
        case ',':
        {
            ins.opcode = READ;
            break;
        }
        case '[':
        {
            ins.opcode = STARTLOOP;
            break;
        }
        case ']':
        {
            ins.opcode = ENDLOOP;
            break;
        }
        }
        optimizedList.push_back(ins);
    }
    unsigned int NumOptPasses = 0;
    // Start optimizing
    size_t currentSize = optimizedList.size();
    size_t newSize = 0;
    // Do while needs to be optmized
    // Really simple (to me at least) optimizations.
    while (newSize - currentSize != 0)
    {
        NumOptPasses++;
        currentSize = optimizedList.size();
        for (size_t i = 0; i < optimizedList.size(); i++)
        {
            if (optimizedList[i].opcode == STARTLOOP && optimizedList.size() > i + 2)
            {
                if (optimizedList[i + 1].opcode == DECBY || optimizedList[i + 1].opcode == INCBY && optimizedList[i + 1].data == 1)
                {
                    if (optimizedList[i + 2].opcode == ENDLOOP)
                    {
                        optimizedList[i].opcode = SETCELL;
                        optimizedList[i].data = 0;

                        optimizedList.erase(optimizedList.begin() + i + 1);
                        optimizedList.erase(optimizedList.begin() + i + 1);

                    }
                }
            }
            if (optimizedList[i].opcode == INCBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == DECBY)
                {
                    int sub = optimizedList[i + 1].data - optimizedList[i].data;
                    if (sub < 0)
                    {
                        optimizedList[i].opcode = INCBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub >= 1)
                    {
                        optimizedList[i].opcode = DECBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub == 0)
                    {
                        optimizedList.erase(optimizedList.begin() + i);
                        optimizedList.erase(optimizedList.begin() + i);
                    }
                }
            }
            if (optimizedList[i].opcode == DECBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == INCBY)
                {
                    int sub = optimizedList[i + 1].data - optimizedList[i].data;
                    if (sub < 0)
                    {
                        optimizedList[i].opcode = DECBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub >= 1)
                    {
                        optimizedList[i].opcode = INCBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub == 0)
                    {
                        optimizedList.erase(optimizedList.begin() + i);
                        optimizedList.erase(optimizedList.begin() + i);
                    }
                }
            }
            if (optimizedList[i].opcode == INCBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == INCBY)
                {
                    optimizedList[i].opcode = INCBY;
                    optimizedList[i].data = optimizedList[i + 1].data + optimizedList[i].data;
                    optimizedList.erase(optimizedList.begin() + i + 1);
                }
            }
            if (optimizedList[i].opcode == DECBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == DECBY)
                {
                    optimizedList[i].opcode = DECBY;
                    optimizedList[i].data = optimizedList[i + 1].data + optimizedList[i].data;
                    optimizedList.erase(optimizedList.begin() + i + 1);
                }
            }
            if (optimizedList[i].opcode == INCCELLBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == DECCELLBY)
                {
                    int sub = optimizedList[i + 1].data - optimizedList[i].data;
                    if (sub < 0)
                    {
                        optimizedList[i].opcode = INCCELLBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub >= 1)
                    {
                        optimizedList[i].opcode = DECCELLBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub == 0)
                    {
                        optimizedList.erase(optimizedList.begin() + i);
                        optimizedList.erase(optimizedList.begin() + i);
                    }
                }
            }
            if (optimizedList[i].opcode == DECCELLBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == INCCELLBY)
                {
                    int sub = optimizedList[i + 1].data - optimizedList[i].data;
                    if (sub < 0)
                    {
                        optimizedList[i].opcode = DECCELLBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub >= 1)
                    {
                        optimizedList[i].opcode = INCCELLBY;
                        optimizedList[i].data = abs(sub);
                        optimizedList.erase(optimizedList.begin() + i + 1);
                    }
                    else if (sub == 0)
                    {
                        optimizedList.erase(optimizedList.begin() + i);
                        optimizedList.erase(optimizedList.begin() + i);
                    }
                }
            }
            if (optimizedList[i].opcode == INCCELLBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == INCCELLBY)
                {
                    optimizedList[i].opcode = INCCELLBY;
                    optimizedList[i].data = optimizedList[i + 1].data + optimizedList[i].data;
                    optimizedList.erase(optimizedList.begin() + i + 1);
                }
            }
            if (optimizedList[i].opcode == DECCELLBY && optimizedList.size() > i + 1)
            {
                if (optimizedList[i + 1].opcode == DECCELLBY)
                {
                    optimizedList[i].opcode = DECCELLBY;
                    optimizedList[i].data = optimizedList[i + 1].data + optimizedList[i].data;
                    optimizedList.erase(optimizedList.begin() + i + 1);
                }
            }
        }
        newSize = optimizedList.size();
    }
    // End optimizing
    // FINAL step - set up jumps for each loop
    for (int i = 0; i < optimizedList.size(); i++)
    {
        switch (optimizedList[i].opcode) {
        case STARTLOOP:
        {
            unsigned int matchCounter = 1;
            unsigned int peek;
            for (peek = i + 1; matchCounter != 0; peek++)
            {
                if (optimizedList[peek].opcode == STARTLOOP)
                {
                    matchCounter++;
                }
                else if (optimizedList[peek].opcode == ENDLOOP)
                {
                    matchCounter--;
                }
            }
            optimizedList[i].data = peek - 1;
            break;
        }
        case ENDLOOP:
        {
            unsigned int matchCounter = 1;
            unsigned int peek;
            for (peek = i - 1; matchCounter != 0; peek--)
            {
                if (optimizedList[peek].opcode == STARTLOOP)
                {
                    matchCounter--;
                }
                else if (optimizedList[peek].opcode == ENDLOOP)
                {
                    matchCounter++;
                }
            }
            optimizedList[i].data = peek + 1;
            break;
        }
        }
    }
    return optimizedList;
}

// Run optimized BrainF program
void RunOptimized(vector<Instruction> optimizedList)
{
    unsigned char* machine = static_cast<unsigned char*>(malloc(MACHINE_SIZE));
    size_t currentCell = 0;
    if (machine != NULL)
    {
        memset(machine, 0, MACHINE_SIZE);
    }
    else
    {
        cout << "Error allocating cells!" << endl;
        exit(-1);
    }

    for (size_t pc = 0; pc < optimizedList.size(); pc++)
    {
        switch (optimizedList[pc].opcode)
        {
        case SETCELL:
        {
            machine[currentCell] = optimizedList[pc].data;
            break;
        }
        case GOTOCELL:
        {
            currentCell = optimizedList[pc].data;
            break;
        }
        case INCCELLBY:
        {
            currentCell += optimizedList[pc].data;
            break;
        }
        case DECCELLBY:
        {
            currentCell -= optimizedList[pc].data;
            break;
        }
        case INCBY:
        {
            machine[currentCell] += optimizedList[pc].data;
            break;
        }
        case DECBY:
        {
            machine[currentCell] -= optimizedList[pc].data;
            break;
        }
        case WRITE:
        {
            cout << machine[currentCell];
            break;
        }
        case READ:
        {
            machine[currentCell] = getchar();
            break;
        }
        case STARTLOOP:
        {
            if (machine[currentCell] == 0)
            {
                pc = optimizedList[pc].data;
            }
            break;
        }
        case ENDLOOP:
        {
            if (machine[currentCell] != 0)
            {
                pc = optimizedList[pc].data;
            }
            break;
        }
        }
    }
    free(machine);
}

// Old code - just to compare speed to optimized version. Runs BF string directly
void RunRaw(string prog)
{
    unsigned char* machine = static_cast<unsigned char*>(malloc(MACHINE_SIZE));
    if (machine != NULL)
    {
        memset(machine, 0, MACHINE_SIZE);
    }
    else
    {
        cout << "Error allocating cells!" << endl;
        exit(-1);
    }
    size_t currentCell = 0;
    for (size_t pc = 0; pc < prog.size(); pc++)
    {
        switch (prog[pc])
        {
        case '>':
        {
            currentCell++;
            break;
        }
        case '<':
        {
            currentCell--;
            break;
        }
        case '+':
        {
            machine[currentCell]++;
            break;
        }
        case '-':
        {
            machine[currentCell]--;
            break;
        }
        case '.':
        {
            cout << machine[currentCell];
            break;
        }
        case ',':
        {
            machine[currentCell] = getchar();
            break;
        }
        case '[':
        {
            if (machine[currentCell] == 0)
            {
                int matchCounter = 1;
                size_t peek;
                for (peek = pc + 1; matchCounter != 0; peek++)
                {
                    if (prog[peek] == '[')
                    {
                        matchCounter++;
                    }
                    else if (prog[peek] == ']')
                    {
                        matchCounter--;
                    }
                }
                pc = peek - 1;
            }
            break;
        }
        case ']':
        {
            if (machine[currentCell] != 0)
            {
                int matchCounter = 1;
                size_t peek;
                for (peek = pc - 1; matchCounter != 0; peek--)
                {
                    if (prog[peek] == '[')
                    {
                        matchCounter--;
                    }
                    else if (prog[peek] == ']')
                    {
                        matchCounter++;
                    }
                }
                pc = peek + 1;
            }
            break;
        }
        }
    }
    free(machine);
}

// Old but currently only version that works - Compiles bf string directly to a string containing C++ but with no optimizations (should be fine tbh the C++ compiler is really good)
string CompileRawToCPP(string prog)
{
    string compiled = "// Generated by MindForn\n//\n\n#include <iostream>\nusing namespace std;int main(int argc, char* argv[]){unsigned char m[" + to_string(MACHINE_SIZE) + "];memset(m, 0, " + to_string(MACHINE_SIZE) + ");unsigned char* c = &m[0];";
    for (size_t progIndex = 0; progIndex < prog.size(); progIndex++)
    {
        switch (prog[progIndex])
        {
        case '>':
        {
            compiled += "c++;";
            break;
        }
        case '<':
        {
            compiled += "c--;";
            break;
        }
        case '+':
        {
            compiled += "++*c;";
            break;
        }
        case '-':
        {
            compiled += "--*c;";
            break;
        }
        case '.':
        {
            compiled += "cout<<*c;";
            break;
        }
        case ',':
        {
            compiled += "*c=getchar();";
            break;
        }
        case '[':
        {
            compiled += "while(*c){";
            break;
        }
        case ']':
        {
            compiled += "}";
            break;
        }
        }
    }
    compiled += "}\n";
    return compiled;
}

int main(int argc, char* argv[])
{
    string prog;
    string outputFile;
    bool compileMode = false;
    // Process arguments
    // Supplying arguments:
    // MindForn.exe -c file.bf file.cpp
    // ^ Compile file.bf to file.cpp
    // MindForn.exe file.bf
    // ^ Run file.bf
    if (argc != 1)
    {
        if (argc == 2)
        {
            if (string(argv[1]) == "-c")
            {
                cout << "Must supply a file to compile!" << endl;
                exit(-1);
            }
            else
            {
                prog = Thruster::TFileToString(Thruster::LoadTFileFromLocalPath(argv[1]));
            }
        }
        else
        {
            if (argc == 4 && string(argv[1]) == "-c")
            {
                compileMode = true;
                prog = Thruster::TFileToString(Thruster::LoadTFileFromLocalPath(argv[2]));
                outputFile = argv[3];
            }
            else if (argc == 3 && string(argv[1]) == "-c")
            {
                cout << "Must supply a file to output!" << endl;
            }
            else
            {
                cout << "Incorrect arguments!" << endl;
                exit(-1);
            }
        }
    }
    else
    {
        prog = textToBrainF("Welcome to MindForn!\nNo program loaded.\nArgs:\n\t-c [file] [output] - compile file to C++\n\t[file path] - run file");
    }
    if (compileMode)
    {
        
        Thruster::TFile file = Thruster::StringToTFile(CompileRawToCPP(prog));
        Thruster::WriteTFileToLocalPath(file, outputFile);
        file.Destroy();
    }
    else
    {
        vector<Instruction> optimizedList = OptimizeProgram(prog);
        RunOptimized(optimizedList);

        //RunRaw(prog);
    }
}
