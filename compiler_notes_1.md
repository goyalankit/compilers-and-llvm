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




