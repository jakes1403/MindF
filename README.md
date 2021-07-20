# MindForn
MindForn - An optimizing BrainF*** language interpreter.

The name of the project is a (slightly) less offensive rewording of BrainF***.

# What is it?
MindForn is my take on running the BrainF*** language, built to decently execute even the slowest of programs at near-compiled speeds.
This is thanks to a technique where the program is analyzed before execution, repeated instructions are combined, dead code is removed, and common constructs are simplified.

Can run any BrainF*** file written in the original version of the language, with currently a max cell count of 30,000.