trwlang
======

simple term rewrite language.

Try!
----

Build kernel.
`clang++ src/kernel.cpp -std=c++11 -Iinclude`

Example
------

By default, the kernel loads rules for binary integer addition and multipulication.

```
In  = Add[1]
Out = 1
In  = Add[1, 2] // binary addition
Out = 3
In  = Add[1, 2, 3] // ternary addition is not implemented.
Out = Add[1, 2, 3]
```

You can define your own rules:

```
In  = Add[x_ ,y_, z_] -> Add[Add[x, y], z]
add new rule
```

Expression `A -> B` is internally translated into `Rule[A, B]`.
When the kernel meets Rule expression, the kernel loads new rule.

When the kernel meets an expression other than Rule, the kernel try to "evaluate" the expresion.
In trwlang, evaluation is done in inverse DFS way.

Here `_` means "Any".
And `x` in `x_` is the name to access matched pattern.
These grammers are similar to Mathematica.

Now you can do ternary addition!

```
In  = Add[1, 2, 3]
Out = 6
```

Similary,

```
In  = Mul[x_ ,y_, z_] -> Mul[Mul[x, y], z]
add new rule
In  = Add[Mul[1, 2], 3, 4]
Out = 9
```

In evaluation, trwlang kernel try to apply rules recursively even after each node is replaced once.
This means that there is the halting problem.

```
In  = f[x_] -> g[x]
add new rule
In  = g[x_] -> f[x]
add new rule
In  = f[1] // infinite loop.
```

TODO
----

More bultin evaluation.
Better data structure.
Array.
