#include <iostream>
#include <math.h>
#include <dolfin>
#include <boost>

//data generator

a = kappa*dot(grad(u), grad(v))*dx;
L = f*v*d;

kappa = Expression(’x[0] + x[1]’, degree=1);
f = Expression(’-8*x[0] - 10*x[1]’, degree=1);