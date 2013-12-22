####Example of Regular Language####

&sum; = {0,1}

`1* = "" + 1 + 11 + 111 + 1111` = all strings of 1

`(1+0)1 = {ab | a E 1+0 ^ b E 1} = {11, 01}` 

`0* + 1* = {0^i | i ≥ 0} U {1^i | i≥0}`

`(0+1)* = U_{i≥0} (0+1)^i = "", (0+1), (0+1)(0+1)` => all strings of 0s and 1s => &sum;*


* Regular Expressions specify regular languages
* Five Constructs: 
  * Two base cases: `empty and 1-character strings`
  * Three compound expressions: union, concatenation, iteration.
* 
