// MindForn : Based on the specs provided in this wikipedia article: 
// https://en.wikipedia.org/wiki/Brainfuck
// Written by Jacob Allen 5/19/2021
//

#include <iostream>
#include <string>
#include "TFile.h"

using namespace std;

const int MACHINE_SIZE = 0xffff;

string textToBrainF(string text)
{
    string code = ">";
    for (int i = 0; i < text.size(); i++)
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
    code += "[-]<";
    return code;
}

int main(int argc, char* argv[])
{
    string prog;
    if (argc != 1)
    {
        if (argc == 2)
        {
            prog = Thruster::TFileToString(Thruster::LoadTFileFromLocalPath(argv[1]));
        }
        else
        {
            cout << "Incorrect arguments!" << endl;
            exit(-1);
        }
    }
    else
    {
        prog = textToBrainF("Welcome to MindForn!\nNo program loaded.\n");
    }
    unsigned char machine[MACHINE_SIZE];
    memset(machine, 0, MACHINE_SIZE);
    unsigned char* current = &machine[0];

    for (int pc = 0; pc < prog.size(); pc++)
    {
        //cout << "Machine:\n";
        //for (int i = 0; i < MACHINE_SIZE; i++)
        //{
        //    cout << "[" << to_string((int)machine[i]) << "] ";
        //}
        //cout << endl;
        switch (prog[pc])
        {
        case '>':
        {
            current++;
            break;
        }
        case '<':
        {
            current--;
            break;
        }
        case '+':
        {
            ++*current;
            break;
        }
        case '-':
        {
            --*current;
            break;
        }
        case '.':
        {
            cout << *current;
            break;
        }
        case ',':
        {
            *current = getchar();
            break;
        }
        case '[':
        {
            if (*current == 0)
            {
                int matchCounter = 1;
                int peek;
                //cout << "----------------\n";
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
                    //cout << "prog " << prog[peek] << "\ncounter " << matchCounter << endl;
                }
                pc = peek - 1;
            }
            break;
        }
        case ']':
        {
            if (*current != 0)
            {
                int matchCounter = 1;
                int peek;
                //cout << "++++++++++++\n";
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
                    //cout << "prog " << prog[peek] << "\ncounter " << matchCounter << endl;
                }
                pc = peek + 1;
                //cout << prog[peek + 1] << endl;
            }
            break;
        }
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
