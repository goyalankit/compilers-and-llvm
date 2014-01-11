####Lexical Analysis####

* Partition the input string into **lexemes**
* Identify the token of each lexeme.
* Left to Right scan => Lookahead required sometimes. Languages try to minimize the amount of lookahead required. 

Example:

C++ Template Syntax: `Foo<Bar>`

C++ Stream Syntax: `cin >> var;`

Nested Templates: `Foo< Bar<Bazz>>` -> here compiler gets confused with `>>`.

Solution: Insert a blank. `> >`

<table><tr><td><a href="compiler_notes_4.md" >&larr; Previous</a></td><td><a href="compiler_notes_6.md" >Next &rarr;</a></td></tr></table>
