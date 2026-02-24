# ESP32 WROOM 32 Elevator Simulation

## Overview
This project simulates a 4-floor elevator system using an ESP32 WROOM 32 microcontroller. The system features call buttons for each floor, LED indicators for the current floor position, a door open indicator, and comprehensive serial output for monitoring elevator status. Completed in February 2026

## Features
✅ 4 tactile momentary switches for floor call buttons
✅ 4 LEDs indicating current elevator floor
✅ 1 LED indicating door open status
✅ Direction tracking (UP/DOWN/IDLE) with SCAN algorithm
✅ Button debouncing for reliable input
✅ Detailed serial output at 115200 baud
✅ 3-second door open duration
✅ 3-second travel time between each floor
✅ Handles continuous button presses correctly

## Hardware Requirements

### Components
- 1x ESP32 WROOM 32 Development Board
- 4x Tactile Momentary Push Button Switches (normally open)
- 5x LEDs (any color, suggest: 4x green for floors, 1x red for door)
- 5x 220Ω - 330Ω Resistors (for LED current limiting)
- 1x Breadboard
- Jumper wires (male-to-male)

### Pin Configuration

| Component | GPIO Pin | Connection Type |
|-----------|----------|-----------------|
| **Floor 1 Button** | GPIO 19 | INPUT_PULLUP |
| **Floor 1 LED** | GPIO 32 | OUTPUT |
| **Floor 2 Button** | GPIO 18 | INPUT_PULLUP |
| **Floor 2 LED** | GPIO 33 | OUTPUT |
| **Floor 3 Button** | GPIO 21 | INPUT_PULLUP |
| **Floor 3 LED** | GPIO 25 | OUTPUT |
| **Floor 4 Button** | GPIO 22 | INPUT_PULLUP |
| **Floor 4 LED** | GPIO 26 | OUTPUT |
| **Door Open LED** | GPIO 27 | OUTPUT |

## Wiring Diagram

### Button Connections
Each button connects between a GPIO pin and GND (using INPUT_PULLUP):

```
ESP32 GPIO Pin ----[Button]---- GND
```

**Specific Connections:**
- GPIO 19 ----[Floor 1 Button]---- GND
- GPIO 18 ----[Floor 2 Button]---- GND
- GPIO 21 ----[Floor 3 Button]---- GND
- GPIO 22 ----[Floor 4 Button]---- GND

### LED Connections
Each LED connects through a current-limiting resistor:

```
ESP32 GPIO Pin ----[220Ω Resistor]----[LED Anode(+)]----[LED Cathode(-)]---- GND
```

**Specific Connections:**
- GPIO 32 ----[Resistor]----[Floor 1 LED]---- GND
- GPIO 33 ----[Resistor]----[Floor 2 LED]---- GND
- GPIO 25 ----[Resistor]----[Floor 3 LED]---- GND
- GPIO 26 ----[Resistor]----[Floor 4 LED]---- GND
- GPIO 27 ----[Resistor]----[Door LED]---- GND

### Power Connections
- Connect ESP32 GND to breadboard ground rail
- Power ESP32 via USB cable connected to computer

## Installation & Setup

### 1. Arduino IDE Setup
1. Install Arduino IDE (version 1.8.x or 2.x)
2. Install ESP32 board support:
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for "ESP32" and install "esp32 by Espressif Systems"

### 2. Board Configuration
- **Board:** "ESP32 Dev Module" or "DOIT ESP32 DEVKIT V1"
- **Upload Speed:** 115200
- **CPU Frequency:** 240MHz (WiFi/BT)
- **Flash Frequency:** 80MHz
- **Flash Mode:** QIO
- **Flash Size:** 4MB
- **Port:** Select your ESP32's COM port

### 3. Upload Code
1. Open `elevator_lab_feb5a.ino` in Arduino IDE
2. Select correct board and port
3. Click **Upload** button
4. Wait for "Done uploading" message

### 4. Open Serial Monitor
1. Go to **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. You should see the welcome message

## Operation

### System Startup
Upon power-up or reset, the system:
1. Initializes all pins
2. Sets elevator to Floor 1
3. Lights up Floor 1 LED
4. Prints welcome message to serial monitor
5. Enters IDLE state, ready to accept calls

### Making a Call
1. Press any floor button (1-4)
2. System registers the call and prints confirmation
3. Elevator begins moving toward the called floor
4. Floor LED updates as elevator moves
5. Elevator pauses 0.5 seconds at each intermediate floor
6. Upon arrival, doors open for 3 seconds
7. Door LED lights during door open period
8. Serial monitor shows current floor and pending calls

### Direction Logic (SCAN Algorithm)
The elevator uses an efficient SCAN algorithm:
- **Moving UP:** Services all floors with calls in upward direction
- **Moving DOWN:** Services all floors with calls in downward direction
- **At Top/Bottom:** Automatically reverses direction if calls exist
- **IDLE:** Remains at current floor until a call is made

This ensures all floors are serviced even if all buttons are continuously pressed.

## Serial Monitor Output

### Example Output
```
================================
ESP32 Elevator Simulation
================================
System initialized
Starting at Floor 1
Ready to accept calls...

>>> Call registered for Floor 3
Elevator arriving at Floor 2
Direction: UP
Elevator arriving at Floor 3
Direction: UP

=== DOORS OPENING ===
Current Floor: 3
Pending Calls: [Floor 1: NO, Floor 2: NO, Floor 3: NO, Floor 4: NO]
=====================

Doors closing...
```

### Output Descriptions
- **Call registered:** Button press detected and call added to queue
- **Elevator arriving at Floor X:** Elevator has reached floor X
- **Direction:** Current movement direction (UP/DOWN/IDLE)
- **DOORS OPENING:** Door sequence initiated
- **Current Floor:** Floor where doors are open
- **Pending Calls:** Shows which floors have active call requests
- **Doors closing:** 3-second door open period completed

## Testing Procedures

### Test 1: Single Floor Call
1. Press Floor 3 button
2. **Expected:** Elevator moves 1→2→3, pausing 0.5s at Floor 2
3. **Expected:** At Floor 3, doors open for 3s, then close

### Test 2: Multiple Calls Same Direction
1. Press Floor 2 button
2. Press Floor 4 button
3. **Expected:** Elevator visits Floor 2 first, then Floor 4

### Test 3: Opposite Direction Calls
1. From Floor 1, press Floor 4 button
2. While moving up, press Floor 1 button
3. **Expected:** Elevator completes trip to Floor 4, then returns to Floor 1

### Test 4: All Buttons Pressed
1. Press and hold all four buttons
2. **Expected:** Elevator systematically visits all floors
3. **Expected:** Direction alternates (UP to top, DOWN to bottom)
4. **Expected:** No floor is skipped or starved

### Test 5: Call During Door Open
1. Elevator at Floor 2 with doors open
2. Press Floor 4 button
3. **Expected:** Call registered, serviced after doors close

## Troubleshooting

### Issue: Buttons Not Responding
- **Check:** Button wiring to correct GPIO pins
- **Check:** Buttons connected to GND
- **Check:** INPUT_PULLUP configuration in code
- **Try:** Adjust DEBOUNCE_DELAY if needed (currently 50ms)

### Issue: LEDs Not Lighting
- **Check:** LED polarity (anode to resistor, cathode to GND)
- **Check:** Resistor values (220Ω - 330Ω recommended)
- **Check:** GPIO pin connections match code
- **Check:** Common ground connection

### Issue: Multiple Calls from Single Press
- **Solution:** Increase DEBOUNCE_DELAY value in code
- **Current:** 50ms (line 30)
- **Try:** 75ms or 100ms

### Issue: Serial Monitor Shows Garbage
- **Check:** Baud rate set to 115200
- **Check:** Correct COM port selected
- **Try:** Press ESP32 reset button

### Issue: Elevator Skips Floors
- **Check:** FLOOR_PAUSE_TIME is set to 3000ms (line 30)
- **Check:** Direction logic in getNextFloor() function
- **Review:** Serial output for direction changes

### Issue: Buttons Not Responding During Travel
- **Solution:** Code uses non-blocking delays
- **Check:** Buttons are continuously monitored during 3-second travel time
- **Check:** Buttons are continuously monitored during 3-second door open time

### Issue: ESP32 Won't Upload
- **Try:** Hold BOOT button while clicking Upload
- **Check:** Correct board selected in Tools menu
- **Check:** USB cable supports data (not charge-only)
- **Try:** Different USB port

## Code Structure

### Main Components
1. **Pin Configuration** (lines 14-30): GPIO pin assignments
2. **Timing Constants** (lines 32-36): Debounce, door, and pause times
3. **State Variables** (lines 68-78): Elevator state tracking
4. **Setup Function** (lines 100-115): Initialization
5. **Main Loop** (lines 121-146): Continuous operation
6. **Button Handling** (lines 183-217): Debouncing and call registration
7. **Elevator Logic** (lines 223-335): Movement and direction control
8. **Door Operations** (lines 341-360): Door open/close sequences
9. **LED Control** (lines 366-378): Floor indicator management
10. **Serial Output** (lines 384-413): Status reporting

### Key Functions
- `readButtons()`: Debounced button reading
- `registerCall(floor)`: Add floor to call queue
- `getNextFloor()`: Determine next destination
- `moveToFloor(target)`: Execute floor traversal
- `openDoors()`: Door sequence with status output
- `updateFloorLED(floor)`: Control floor indicators

## Customization

### Timing Adjustments
Modify these constants to change behavior:

```cpp
const unsigned long DEBOUNCE_DELAY = 50;      // Button debounce time
const unsigned long DOOR_OPEN_TIME = 3000;    // Door open duration
const unsigned long FLOOR_PAUSE_TIME = 3000;  // Travel time between floors
```

### Pin Changes
If you need different GPIO pins, update these constants:

```cpp
const int BUTTON_FLOOR_1 = 19;  // Change to your pin
const int LED_FLOOR_1 = 32;     // Change to your pin
// ... etc
```

### Serial Baud Rate
To change serial communication speed:

```cpp
Serial.begin(115200);  // Change to desired baud rate
```

## Technical Specifications

- **Microcontroller:** ESP32 WROOM 32
- **Operating Voltage:** 3.3V (GPIO)
- **Input Method:** INPUT_PULLUP (active LOW)
- **Debounce Time:** 50ms
- **Door Open Duration:** 3 seconds
- **Floor Transit Time:** 3 seconds per floor
- **Button Monitoring:** Continuous (non-blocking delays)
- **Serial Baud Rate:** 115200
- **Algorithm:** SCAN (elevator scheduling)

## Safety Notes

⚠️ **Important:**
- ESP32 GPIO pins are 3.3V - do NOT connect to 5V
- Always use current-limiting resistors with LEDs
- Verify pin connections before powering on
- Use appropriate resistor values for your LEDs (typically 220Ω-330Ω)

## License
This code is provided for educational purposes.

## Support
For issues or questions:
1. Check the Troubleshooting section
2. Verify wiring matches pin configuration
3. Review serial monitor output for debugging
4. Check that all components are functioning individually

## Version History
- **v1.0** - Initial release with full elevator simulation functionality
