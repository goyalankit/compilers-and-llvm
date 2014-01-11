####Lecture 2####

Structure of a Compiler

1. **Lexical Analysis**
2. **Parsing**
3. **Semantic Analysis**
4. **Optimization**
5. **Code Generation**

---

**First Step**: Recognize words

* **Lexical Analysis** : *divides program text into "words" or "tokens".*

`if x==y then z=1; else z=2` contains *Keywords, operators, variable names, tokens*

Parsing: 


![Lexical Analysis](https://gist.github.com/goyalankit/8074196/raw/85664326f4931a96082d62b55b95bc306bafc044/compilers1.png)


* **Semantical Analysis** : compilers perform limited semantical analysis. This is too hard.

`Jack said Jack left his assignment at home?` : How many people? Worse case 3 different people.

Programming languages define strict rules to prevent such ambiguities. Analogy:

```
{
  int Jack = 2;
  {
    int Jack = 4;
    cout << Jack;
  }
}

```

* **Optimization** is a little like editing.

Automatically modify program so that they:
  * Run Faster
  * Use less memory
  * less power use
  * number of network messages
  * number of database calls
  
`X = Y * 0 is the same as X = 0` => **INCORRECT RULE**

Above rule is valid only for integers. Invalid for floating points.
because of the rule `NAN * 0 = NAN`

* **Code Generation**

Translation to another language. High level code -> Assembly Code.

---

Proportions of each phase hass changed over time.

Then: `[....L....] [....P.....] [.S.] [....O....] [....CG....]`

Now: `[.L.][.P.][.......S......][...........O............][.CG.]`


