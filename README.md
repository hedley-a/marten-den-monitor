## Marten den box temperature and occupancy logger
- Objectives: Low cost, reliable and easy to depoly.
- Improve the method of detecting occupancy used in the North American Joyce study

![grafik](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/images/DenBox_front.jpg)
## Marten box inside view
![grafik](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/images/DenBox_interior.jpg)
  
## [Joyce Study](https://stacksjournal.org/wp-content/uploads/2025/06/Joyce-25003-Article.pdf)
   _"Each den box had an internal temperature logger (HOBO® UA-001-64
     pendant logger or MX2201, Onset Computer Corporation, Massachusetts,
     USA) that was screwed to an inside wall roughly 5 cm above the top of the
     wood chip bedding material, close to the bottom of boxes so that animals resting
     in den boxes would be more likely to heat the air around
     temperature loggers. "_  
     
Temperatures were sampled every 5 minutes over 4 months.  
Potential use by an animal was identified by searching for temperature changes using **absolute den temperature data only**  
The methodology is explained in detail within the paper.  

## Proposed Improvement
A custom built logger running our own firmware designed for easy deployment
### 
### Data recovery
- Data downloadable using bluetooth [BLE] to mobile device
- Ground level access, no animal disturbance.
- Commercial BLE loggers are available, but cost is prohibitive and functionality limited
## Hardware
Seed XIAO Nordic Semiconductor NRF52840 processor module.
-  2 MB megabytes of on-board flash memory
- Sufficent for 485 days of data, logging 4 bytes of data every 1 minute
- That 4 bytes can be jibbled about to share between sensors.  
Integrated Bluetooth [BLE]
- Operable in micropower mode
- Up to three stainless steel probe temperature sensors: NTC thermistors  
- SHT41: ambient temperature sensor
- ABS box
- 2 AAA Lithium iron disulphide cells and holder
- Approx £35 as hand built prototype, maybe cheaper in quantity.

#### Example sensor allocation
| Sensor                    | Bits | Integer Range | True Value Range              |Notes                      |
| --------------------------|------|---------------|---------------------|-------------------------------------|
|Temp 1 (Absolute)          |	9    |	0-511        | -40.0°C to +62.2°C  |(0.2°C Res)                          |
|Temp 2 (difference from T1)|	7		 |  0-127        | -12.0°C to +12.0°C  |0.2°C resolution (relative to Temp 1)|
|Temp 3 (Offset from T1)    |	6		 |  0-31         | - 8.0°C to +23.0°C  |1.0°C resolution (relative to Temp 1)|
|Temp 4 (Absolute)          |	8		 |  0-255        | -40.0°C to +62.2°C  |0.4°C resolution                     |
|System Flags               |	2		 |  0-3          |                     | flag rapid temperature changes?     |         
|Total	                    | 32	 |4 Bytes        |                     |                                     |
  
#### Derived parameters (rate of temperature change) 
The Joyce study used one sensor reduce equipment cost.  
Rate of change analysis was carried out post data recovery to identify occupancy events.  

I propose that:    
We use at least two sensors (in our case adds mimimal cost)  
We consider applying the Joyce rate of change equations within the logging process; 
R-values and Z-values would be calcualted and stored present in the downloaded data
- to the absolute den temperature 
- to the difference between den and ambient reference.   

### Logger health checks
- Battery voltage will be sampled at 1 minute intervals and daily max/min logged (2 bytes)
- The internal humidity will like-wise be max/min logged (2 bytes) Why? **Leak detection!**    
 
### Recommendations
 Differential temperature storage is the key to sdata efficiently.
- Continuous logging provides sufficient storage duration whilst simplifying software.
- There is a risk with triggering that events are missed or spurious data recorded.
- **'Confidence factor'** Derived from of change of temperature and absolute difference coded with care.
- Temperature analysis to determine box occupancy may become unreliable during periods of high ambient temperature.   
### Risks
- nRF52 development kit for debugging / programming  - working, tested
- Plenty of boiler-plate code and the device is IMO *'just right for the job'*
- 'Hello world' BLE (bluetooth low energy) connection with Nordic smartphone APP - tested
- IANAE but with a bit of help (not all, hopefully from AI) this project should be do-able.

### Design constraints:
- **EEPROM memory** is cheap, low power, easy to integrate, limited capacity.  
- EEPROM requires remote download - adds to design complexity but advantage for users.
- In principle 8*512KBit EEPROMS could be addressed by one nRF52 device (4KBit of memory). 
-**Lithium/Iron Disulfide battery** (AAA Size * 2)
- 1 Ah capacity 1.4 Watt/hours * 2  - 2.4 Watt/hours
- Stable voltage matched to nRF52 and memory modules
- Predicted life 3-4 years
   
### Deployment:
Two 3 mm holes drilled into the base of the box on either side of one internal nest-box baffle
Probe/logger assembly robust, simple to deploy and service.  
Data - over one year of storage - download by Bluetooth 

![grafik](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/images/DenboxDrg.png)  

![grafik](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/images/ProbeLocn.png) 

## Development
- Development: box containing 'dummy marten' to optimise probe location and evaluate data.
## [Dummy Marten evaluation]

## [Box modification drawing](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/MonitorBox.pdf)

## [Circuit diagram](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/Schematic_Den-Box-Logger_2026-10-28.pdf)








