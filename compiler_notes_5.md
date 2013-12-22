####Lexical Analysis####

C++ Template Syntax: `Foo<Bar>`

C++ Stream Syntax: `cin >> var;`

Nested Templates: `Foo< Bar<Bazz>>` -> here compiler gets confused with `>>`.

Solution: Insert a blank. `> >`