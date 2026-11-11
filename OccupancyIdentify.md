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
• Denominator Alignment for \(Z_{i}\): Because \(R[2]\) is centred at \(t_{min}[3]\) and \(R[0]\) is centred at \(t_{min}[1]\), Equation 2 divides R[2] - R[0] by t_min[3] - t_min[1]  
(which is ~2 minutes).  
• Power Saving: For battery-powered logging on the Seeed XIAO nRF52840,replace the millis() polling loop with delay(60000)—on the nRF52 mbed/Adafruit core, delay()  
automatically puts the CPU into low-power sleep mode between samples. 

### 3. Integer-Only C++ Implementation
Adding two 6-bit temperature deltas (11 + 6 + 6 = 23 bits) leaves 9 bits for Z, packing all four values into a single 32-bit (uint32_t, 4-byte) flash record.  
Using 6 signed bits per delta gives a range of -32 °C to +31 °C.

1. 32-Bit Record Layout (uint32_t)  

Packing sample into a standard 4-byte word aligns with flash memory pages on the Seeed nRF52:  

|Field	             |Bits	    |Bit    |Resolution	     |Encoded Range                                                |
|--------------------|----------|-------|----------------|-------------------------------------------------------------|
T1 (Primary Temp)    |	11 bits	|0 – 10	|0.1 °C          |	-40.0 °C to +60.0 °C (stored as 0 to 1000 with +400 offset)|
ΔT2 (T2 - T1)        |	6 bits	|11 – 16|	1 °C         |	-32 °C to +31 °C (signed two's complement)|
ΔT3 (T3 - T1)        |	6 bits	|17 – 22|	1 °C         |	-32 °C to +31 °C (signed two's complement)|
Z_scaled (2nd Deriv) |	9 bits	|23 – 31|	0.025 °C/min²|	-6.400 to +6.375 °C/min² (-256 to +255 raw)|

• Wider Z Range Option: If your cavity experiences thermal shocks faster than ±6.4 °C/min², you can either divide Z_scaled by 2 (0.05 °C/min² steps, giving ±12.8 °C/min² in 9 bits) or use a 5-byte (40-bit) struct to give Z 17 bits.
• Timestamp Alignment: Because Z is centred 2 minutes behind the current sample, T2 and T3 must also be buffered in 5-sample arrays so all four packed values come from the exact same minute (t - 2 min).

2. C++ Bit-Packing & Unpacking Code

This sketch buffers all three sensors, rounds the 0.1 °C differences to the nearest 1 °C, clamps every value so an out-of-range reading never corrupts neighbouring bits, and packs the record into one uint32_t:


```
    #include <Arduino.h>
    
    const unsigned long SAMPLE_INTERVAL_MS = 60000UL;
    unsigned long lastSampleTime = 0;
    
    // 5-sample sliding windows (index 0 = oldest, index 4 = newest)
    // All temperatures stored in tenths of a degree (-400 to +600)
    int16_t T1[5] = {0};
    int16_t T2[5] = {0};
    int16_t T3[5] = {0};
    
    // First derivative of T1 in 0.05 °C/min units
    int16_t R[3] = {0};
    int sampleCount = 0;
    
    // Replace with your actual sensor reads (in 0.1 °C units)
    void readAllTemperatures(int16_t &t1, int16_t &t2, int16_t &t3) {
      t1 = 254; // 25.4 °C
      t2 = 112; // 11.2 °C (delta = -14 °C)
      t3 = 416; // 41.6 °C (delta = +16 °C)
    }
    
    // Helper: compute rounded 1 °C difference and clamp to 6-bit signed [-32, 31]
    uint32_t encodeDelta6Bit(int16_t tx_tenths, int16_t t1_tenths) {
      int16_t diff_tenths = tx_tenths - t1_tenths;
      // Round to nearest integer degree instead of truncating
      int16_t diff_deg = (diff_tenths >= 0) ? ((diff_tenths + 5) / 10)
                                            : ((diff_tenths - 5) / 10);
    
      if (diff_deg < -32) diff_deg = -32;
      if (diff_deg > 31)  diff_deg = 31;
    
      return (uint32_t)(diff_deg & 0x3F);
    }
    
    // Pack T1, ΔT2, ΔT3, and Z_scaled into a single 32-bit word
    uint32_t packRecord(int16_t t1_tenths, int16_t t2_tenths, int16_t t3_tenths, int16_t z_scaled) {
      // 1. Offset T1 by +400 so -40.0 °C becomes 0 (fits cleanly in 11 bits: 0..1000)
      int16_t t1_offset = t1_tenths + 400;
      if (t1_offset < 0)    t1_offset = 0;
      if (t1_offset > 2047) t1_offset = 2047;
      uint32_t u_t1 = (uint32_t)(t1_offset & 0x7FF);
    
      // 2. Encode 6-bit deltas
      uint32_t u_dt2 = encodeDelta6Bit(t2_tenths, t1_tenths);
      uint32_t u_dt3 = encodeDelta6Bit(t3_tenths, t1_tenths);
    
      // 3. Clamp Z_scaled to 9-bit signed [-256, 255]
      if (z_scaled < -256) z_scaled = -256;
      if (z_scaled > 255)  z_scaled = 255;
      uint32_t u_z = (uint32_t)(z_scaled & 0x1FF);
    
      return u_t1 | (u_dt2 << 11) | (u_dt3 << 17) | (u_z << 23);
    }
    
    // Unpack a 32-bit record back into real units when reading flash
    void unpackRecord(uint32_t record, float &t1_c, float &t2_c, float &t3_c, float &z_val) {
      int16_t t1_tenths = (int16_t)(record & 0x7FF) - 400;
    
      // Sign-extend 6-bit deltas
      int8_t dt2 = (int8_t)((record >> 11) & 0x3F);
      if (dt2 & 0x20) dt2 |= 0xC0;
    
      int8_t dt3 = (int8_t)((record >> 17) & 0x3F);
      if (dt3 & 0x20) dt3 |= 0xC0;
    
      // Sign-extend 9-bit Z
      int16_t z_scaled = (int16_t)((record >> 23) & 0x1FF);
      if (z_scaled & 0x100) z_scaled |= 0xFE00;
    
      t1_c  = t1_tenths / 10.0f;
      t2_c  = t1_c + dt2;
      t3_c  = t1_c + dt3;
      z_val = z_scaled / 40.0f;
    }
    
    void logToFlash(uint32_t packedRecord) {
      // Write 4 bytes directly to your QSPI / LittleFS file:
      // file.write((uint8_t*)&packedRecord, sizeof(packedRecord));
    }
    
    void setup() {
      Serial.begin(115200);
    }
    
    void loop() {
      unsigned long currentMillis = millis();
    
      if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL_MS || sampleCount == 0) {
        lastSampleTime = currentMillis;
    
        // Shift all three temperature histories left
        for (int i = 0; i < 4; i++) {
          T1[i] = T1[i + 1];
          T2[i] = T2[i + 1];
          T3[i] = T3[i + 1];
        }
    
        readAllTemperatures(T1[4], T2[4], T3[4]);
        sampleCount++;
    
        if (sampleCount >= 3) {
          R[0] = R[1];
          R[1] = R[2];
          R[2] = T1[4] - T1[2];
        }
    
        if (sampleCount >= 5) {
          int16_t Z_scaled = R[2] - R[0];
    
          // Pack aligned samples from index [2] (t - 2 min)
          uint32_t packed = packRecord(T1[2], T2[2], T3[2], Z_scaled);
          logToFlash(packed);
    
          // Verify by unpacking
          float t1, t2, t3, z;
          unpackRecord(packed, t1, t2, t3, z);
          Serial.printf("Packed: 0x%08lX | T1: %.1fC, T2: %.0fC, T3: %.0fC, Z: %.3f\n",
                        packed, t1, t2, t3, z);
        }
      }
    }
```
End OF Code



