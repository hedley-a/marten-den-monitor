## Temperature Derivative Equations
The Joyce study used two rate of change equations to help distinguish  
sudden changes in internal temperature from normal daily oscillations in temperature. 

### 1. First Derivative: Rate of Temperature Change

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

##Coding
Log three temperatures - The occupancy sensor -40 to +60 deg.C @ 0.1 deg.C resolution
Two differences ambient and top 

Use fixed-point integer math (int16_t or int32_t) for logging to flash: saves storage space, prevents rounding drift, and executes faster on the nRF52.

• Actual Elapsed Time (t_min): Instead of assuming the denominator is always exactly 2.0 minutes, the code divides millis() by 60000.0f. 
This prevents drift errors if sensor reading or SD/BLE logging takes a few extra milliseconds.
• Denominator Alignment for \(Z_{i}\): Because \(R[2]\) is centred at \(t_{min}[3]\) and \(R[0]\) is centred at \(t_{min}[1]\), Equation 2 divides R[2] - R[0] by t_min[3] - t_min[1] (which is ~2 minutes).
• Power Saving Tip: For battery-powered logging on the Seeed XIAO nRF52840, you can replace the millis() polling loop with delay(60000)—on the nRF52 mbed/Adafruit core, delay() automatically puts the CPU into low-power sleep mode between samples.
