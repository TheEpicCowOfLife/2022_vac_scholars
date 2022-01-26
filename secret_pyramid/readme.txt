So I wanted to see how douglas ratchford would work on this existing competitive programming problem
So I just hastily implemented it to see what would happen, and compared it against my old (heavily optimised) simulated annealing code
There might be bugs in my douglas ratchford code still, but I am too lazy to debug.

https://orac2.info/problem/fario07giza/


Observations:
It simply does not perform as well. Orac2 currently scores on min, and not average, so I don't have numbers yet,
but it seems to score about 85-90%, while simulated annealing scores 100%.
I might try to visualise it and see if it is an implementation issue- DR doesn't solve sample case for whatever reason.
