<img align="left" src="https://github.com/jakes1403/MindForn/blob/main/res/mflogo.png?raw=true" width="512" height="512"/>

# MindF
MindF - An optimizing BrainF*** language interpreter.

The name of the project is a (slightly) less offensive rewording of BrainF***.

# What is it?
MindF is my take on running the BrainF*** language, built to decently execute even the slowest of programs at near-compiled speeds.
This is thanks to a technique where the program is analyzed before execution, repeated instructions are combined, dead code is removed, and common constructs are simplified.

Can run any BrainF*** file written in the original version of the language, with currently a max cell count of 30,000.
