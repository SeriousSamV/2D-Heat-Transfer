2D-Heat-Transfer
================
##Theory - Physics
When we divide a 2-D plate which is in steady state condition(ie. there is no more temperature variation in a point with respect to time), We find:

```math
temp = f(posX, posY)
temp(m, n) approx= (temp(m, n-1) + temp(m, n+1) + temp(m-1, n) + temp(m+1, n)) / 4
```

The above postulate stands true if the heat transfer is only through Conduction(i.e. there is no heat transfer through convection or radiation).