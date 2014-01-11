Regular Languages
=================

set of strings belongs to token use regular languages.

Use regular expressions to define regular languages.

* Single character

  ` 'c' = {"c"} `

* Epsilon

  `E = { "" }` ≠ Empty

* Union 

  `A + B = {a| a E A} U {b| b E B}`

* Concatenation 

  `AB = {ab | a E A ^ b E B}`
  
* Iteration

  `A* = U_{i>o} Ai` where Ai = A...A; A0 = ""
  
**Definition:** The Regular Expressions over &sum; are the smallest set of expressions including

Known as grammar.

```
  R = E
    | 'c' where c E &sum;
    | 'R + R'
    | RR
    | R*
```