## Temperature Derivative Equations

### 1. First Derivative: Rate of Temperature Change ($R_i$)

Equation 1 calculates the difference in temperature over time:

$$R_i = \frac{\Delta T}{\Delta t} = \frac{T_{i+1} - T_{i-1}}{t_{i+1} - t_{i-1}}$$

where:
* $R_i$ is the rate of change in cavity temperature with respect to time ($T/t$) at time step $i$.
* $T_{i+1}$ is the temperature at the next time step.
* $T_{i-1}$ is the temperature at the previous time step.
* $t_{i+1}$ is the time at the next time interval.
* $t_{i-1}$ is the time at the previous time interval.

$R$ is the first derivative of temperature ($T$) with respect to time ($dT/dt$).

---

### 2. Second Derivative: Rate of Change in $R$ ($Z_i$)

Equation 2 calculates the difference in $R$ over time:

$$Z_i = \frac{\Delta R}{\Delta t} = \frac{R_{i+1} - R_{i-1}}{t_{i+1} - t_{i-1}}$$

where:
* $Z_i$ is the second derivative of temperature ($T$) and the first derivative of the rate of change ($R$) with respect to time ($T/t^2$), at time step $i$.
* $R_{i+1}$ is the rate at the next time step.
* $R_{i-1}$ is the rate at the previous time step.
* $t_{i+1}$ is the time at the next time interval.
* $t_{i-1}$ is the time at the previous time interval.

Equation 2 minimizes variability in $R$ over time, which further helps distinguish sudden changes in temperature.
