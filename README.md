# Regex

## Task
With given regular expression and string find substring
that conforms to regular expression with maximal length.

## Algorithm
It's pretty straightforward:
- Parse regular expression to finite automata
- Get rid of EPS-transitions in automata
- For k from length of string to 0 for every substring of length k \
in string check if it conforms to regular expression \
by running BFS in automata

## Usage
```bash
USAGE: ./solve REGEX STR
Outputs substring of STR with maximum len
such that it conforms to REGEX.
```

## Compiling
```bash
make
```

## Running tests and coverage check
```bash
make test
./tests
```
