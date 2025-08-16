# Mizip credit editor

> [!IMPORTANT]
> This software has been created purely for educational purposes. I do not condone any abuse of this software nor I take responsibility for any illegal activities performed with it.

This little piece of software, written in C++, allows to rewrite the second sector of MiZip vending machines keys. Just dump the current blocks in the software, insert the new credit, rewrite the new sector and you're good to go.
To compile:

```cpp
g++ -o main.cpp edit_credit_mizip.cpp
```
