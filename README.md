LucidCpp is a lightweight, header-only C++ visualization tool designed for debugging and education. It requires zero changes to your existing codebase—no inheriting from base classes or rewriting your structs. Simply #include the library, pass your head pointer to the engine, and it automatically traverses your linear and non-linear data structures to generate clear, visual graphs of your memory.

## Features

- **Header-only**: No compilation or linking required
- **Zero integration**: Works with existing code without modifications
- **Automatic traversal**: Intelligently navigates linear and non-linear data structures
- **Visual output**: Generates clear, easy-to-understand memory graphs

## Quick Start

```cpp
#include "lucidcpp.hpp"

// Pass your data structure head pointer
MyNode* head = /* ... */;
lucidcpp::visualize(head);
```
