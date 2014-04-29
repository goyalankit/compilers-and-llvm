Notes
---

1. **Loop**: A strongly connected component in CFG.

2. **Natural Loop**: Loop with only a single loop header. i.e., NOT able to
enter in the middle of the loop (may be using branch instruction)

3. **Reducible Flow Graph**: CFG whose loops are all natural loops.

---

### Dataflow Analysis

1. Liveness flows backwards through the CFG, **because the behavior at future nodes determines liveness at a given node**
2. Imprecise conservative solutions ⇒ sub-optimal but correct programs

---

### Lattice theory

1. Inferior solutions are lower on the lattice
2. More conservative solutions are lower on the lattice

---


– d dom i if all paths from **entry** to node i include d 

1. The dominance frontier of a node d is the set of nodes that are “just
barely” not dominated by d; i.e., the set of nodes n, such that 

    – d dominates a predecessor p of n, and
    – d does not strictly dominate n


---

### Loop Invariant code motion

– Constant
– Have all reaching definitions outside of L
– Have exactly one reaching definition and that definition is in the set marked “invariant”

1. The block containing s dominates all loop exits
2. No other statement in the loop assigns to x
3. The block containing s dominates all uses of x in the loop


TODO: 
handout 07: slide 30: variable renaming algorithm.



