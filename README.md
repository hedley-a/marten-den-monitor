## Marten den box temperature and occupancylogger
- Objectives: A low cost, reliable and easy to depoly device
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
- Commercial BLE loggers are available, cost is prohibitive.
  
### logged parameters
- **Temperature** outside and inside den box every 5 minutes
- This could be a single point in time, or oversampled +/- trend, maximum or minimum
- **Occupancy Trigger** oversampled data processed then flag occupancy using Δ $\Delta$ temperature signature
- Default to be determined by experiment and then made field reconfigurable
- **Differential temperature**
- Two identical temperature probes, in the den and entrance, gives an accurate difference and occupancy trigger compensation for enviromental changes
- rejecting common mode change: e.g sunlight on box.    
- in a custom design this is simple to implement.
- Reduces the amount of data stored: Logger cost and power consumption savings.
- Less data to recover, analyse and discard post download.

## Hardware
- Nordic Semiconductor NRF52832 processor module with Bluetooth [BLE]
- 1024kilobit - 256 Kilobytes of EEPROM memory (4 x CAT24C512)  
- Passive NTC thermistor differential bridge internal temperature sensors  
- SHT41 Temperature sensor (mounted within bottom box)
- ABS box
- 3 mm diameter stainless steel sensor sleeves
- 2 AAA Lithium iron disulphide cells and holder
- Approx £35 as hand built prototype, maybe cheaper in quantity.
  
### Data Storage:
1024k of EEPROM memory 131,072 Bytes

#### Memory Budget
Keeping hardware as simple as possible requres 
- Logging interval five minutes
- Reduce data to one byte - just the +/- 12°C diff @ 0.1°C resolution.
- **288 bytes/Day**
- Environmental monitoring - every four hours:  Internal, External Max/ Min @ 1°C resolution
- **24 bytes/day**
- No need for timestamps due to regularity.
- **312 bytes/day**
- **131,072/312 = 354 days(approx)**
### Recommendations
 Differential temperature storage is the key to storing data efficiently.
- Continuous logging provides sufficient storage duration whilst simplifying software.
- There is a risk with triggering that events are missed or spurious data recorded.
- it is expected that using temperature analysis to determine box occupancy may become unreliable during periods of high ambient temperature.   
### Risks
- nRF52 development kit for debugging / programming  - working, tested
- There is a plenty of boiler-plate code and the device is IMO *'just right for the job'*
- 'Hello world' BLE (bluetooth low energy) connection with Nordic smartphone APP - tested
- IANAE but with a bit of help (not all, hopefully from AI) this project should be do-able.

### Design constraints:
- **EEPROM memory** is cheap, low power, easy to integrate but with limited capacity.  
- SD cards offer capacity but come with low power low Voltage problems:  
- [thecavepearlproject.org/2017/05/21/switching-off-sd-cards-for-low-power-data-logging/](https://thecavepearlproject.org/2017/05/21/switching-off-sd-cards-for-low-power-data-logging/)
- EEPROM requires remote download - adds to design complexity but advantage for users.
- In principle 6*512KBit EEPROMS could be addressed by one nRF52 device (3KBit of memory). 
-**Lithium/Iron Disulfide battery** (AAA Size * 2)
- 1 Ah capacity 1.4 Watt/hours * 2  - 2.4 Watt/hours
- Stable voltage matched to nRF52 and memory modules
   
### Deployment:
Two 6 mm holes drilled into the base of the box on either side of one internal nest-box baffle
Probe/logger assembly robust, simple to deploy and service.  
Data - one year of storage - download by Bluetooth 
![grafik](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/images/DenboxProbes.png)  

## [Box modification drawing](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/monitor_box.pdf)

## [Circuit diagram](https://github.com/hedley-a/Marten-Logger-BLE/blob/main/schematic_dbl.pdf)

##Further Design options
-- Does require custom hardware / software.
## Radar sensing
- Evaluation of the LD2410S shows promise
- Configurable range and sensitivity
- Low power 40 - 120 microamps - 3.0 - 3.6 Volts.




