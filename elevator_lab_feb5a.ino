/*
 * ESP32 WROOM 32 Elevator Simulation
 * 
 * Features:
 * - 4 floor elevator with call buttons
 * - Floor indicator LEDs
 * - Door open indicator LED
 * - Direction tracking (UP/DOWN/IDLE)
 * - Detailed serial output
 * - Button debouncing
 * - SCAN algorithm for efficient floor servicing
 */

// ============================================
// PIN CONFIGURATION
// ============================================

// Floor 1
const int BUTTON_FLOOR_1 = 19;
const int LED_FLOOR_1 = 32;

// Floor 2
const int BUTTON_FLOOR_2 = 18;
const int LED_FLOOR_2 = 33;

// Floor 3
const int BUTTON_FLOOR_3 = 21;
const int LED_FLOOR_3 = 25;

// Floor 4
const int BUTTON_FLOOR_4 = 22;
const int LED_FLOOR_4 = 26;

// Door indicator
const int LED_DOOR_OPEN = 27;

// ============================================
// TIMING CONSTANTS
// ============================================

const unsigned long DEBOUNCE_DELAY = 50;      // 50ms for button debouncing
const unsigned long DOOR_OPEN_TIME = 3000;    // 3 seconds door open
const unsigned long FLOOR_PAUSE_TIME = 3000;  // 3 seconds between floors

// ============================================
// ENUMERATIONS AND ARRAYS
// ============================================

// Direction states
enum Direction {
  IDLE,
  UP,
  DOWN
};

// Number of floors
const int NUM_FLOORS = 4;

// Arrays for easy iteration
const int buttonPins[NUM_FLOORS] = {BUTTON_FLOOR_1, BUTTON_FLOOR_2, BUTTON_FLOOR_3, BUTTON_FLOOR_4};
const int ledPins[NUM_FLOORS] = {LED_FLOOR_1, LED_FLOOR_2, LED_FLOOR_3, LED_FLOOR_4};

// ============================================
// STATE VARIABLES
// ============================================

// Elevator state
int currentFloor = 1;                          // Start at floor 1
Direction direction = IDLE;                     // Start idle
bool doorOpen = false;                          // Doors closed initially

// Call tracking
bool floorCalls[NUM_FLOORS] = {false, false, false, false};

// Button debouncing
bool lastButtonState[NUM_FLOORS] = {HIGH, HIGH, HIGH, HIGH};
bool buttonState[NUM_FLOORS] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[NUM_FLOORS] = {0, 0, 0, 0};

// ============================================
// FUNCTION DECLARATIONS
// ============================================

void initializePins();
void printWelcomeMessage();
void readButtons();
void registerCall(int floor);
bool hasPendingCalls();
bool hasCallsInDirection(Direction dir);
int getNextFloor();
void moveToFloor(int targetFloor);
void openDoors();
void closeDoors();
void updateFloorLED(int floor);
void turnOffAllFloorLEDs();
void printFloorArrival(int floor);
void printPendingCalls();

// ============================================
// SETUP FUNCTION
// ============================================

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
  
  // Initialize pins
  initializePins();
  
  // Print welcome message
  printWelcomeMessage();
  
  // Set initial floor LED
  updateFloorLED(currentFloor);
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  // Always read buttons
  readButtons();
  
  // Check if there are pending calls
  if (hasPendingCalls()) {
    // Get next floor to visit
    int nextFloor = getNextFloor();
    
    if (nextFloor != -1 && nextFloor != currentFloor) {
      // Move to next floor
      moveToFloor(nextFloor);
    } else if (nextFloor == currentFloor && floorCalls[currentFloor - 1]) {
      // Already at called floor
      openDoors();
      floorCalls[currentFloor - 1] = false;
      closeDoors();
    }
  } else {
    // No calls, set to idle
    direction = IDLE;
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

// ============================================
// INITIALIZATION FUNCTIONS
// ============================================

void initializePins() {
  // Configure button pins
  for (int i = 0; i < NUM_FLOORS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  // Configure LED pins
  for (int i = 0; i < NUM_FLOORS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  // Configure door LED
  pinMode(LED_DOOR_OPEN, OUTPUT);
  digitalWrite(LED_DOOR_OPEN, LOW);
}

void printWelcomeMessage() {
  Serial.println("\n\n================================");
  Serial.println("ESP32 Elevator Simulation");
  Serial.println("================================");
  Serial.println("System initialized");
  Serial.print("Starting at Floor ");
  Serial.println(currentFloor);
  Serial.println("Ready to accept calls...\n");
}

// ============================================
// BUTTON HANDLING FUNCTIONS
// ============================================

void readButtons() {
  for (int i = 0; i < NUM_FLOORS; i++) {
    int reading = digitalRead(buttonPins[i]);
    
    // Check if button state changed
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }
    
    // Check if enough time has passed
    if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      // If state has changed
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        
        // Button pressed (LOW because of INPUT_PULLUP)
        if (buttonState[i] == LOW) {
          registerCall(i + 1); // Convert index to floor number
        }
      }
    }
    
    lastButtonState[i] = reading;
  }
}

void registerCall(int floor) {
  int index = floor - 1; // Convert floor to array index
  
  if (!floorCalls[index]) {
    floorCalls[index] = true;
    Serial.print(">>> Call registered for Floor ");
    Serial.println(floor);
  }
}

// ============================================
// ELEVATOR LOGIC FUNCTIONS
// ============================================

bool hasPendingCalls() {
  for (int i = 0; i < NUM_FLOORS; i++) {
    if (floorCalls[i]) {
      return true;
    }
  }
  return false;
}

bool hasCallsInDirection(Direction dir) {
  if (dir == UP) {
    for (int i = currentFloor; i < NUM_FLOORS; i++) {
      if (floorCalls[i]) return true;
    }
  } else if (dir == DOWN) {
    for (int i = 0; i < currentFloor - 1; i++) {
      if (floorCalls[i]) return true;
    }
  }
  return false;
}

int getNextFloor() {
  if (direction == UP) {
    // Look for calls above current floor
    for (int i = currentFloor; i < NUM_FLOORS; i++) {
      if (floorCalls[i]) {
        return i + 1;
      }
    }
    // No calls above, check below
    if (hasCallsInDirection(DOWN)) {
      direction = DOWN;
      return getNextFloor();
    }
  } else if (direction == DOWN) {
    // Look for calls below current floor
    for (int i = currentFloor - 2; i >= 0; i--) {
      if (floorCalls[i]) {
        return i + 1;
      }
    }
    // No calls below, check above
    if (hasCallsInDirection(UP)) {
      direction = UP;
      return getNextFloor();
    }
  } else if (direction == IDLE) {
    // Find nearest call
    // First check current floor
    if (floorCalls[currentFloor - 1]) {
      return currentFloor;
    }
    
    // Check floors above
    for (int i = currentFloor; i < NUM_FLOORS; i++) {
      if (floorCalls[i]) {
        direction = UP;
        return i + 1;
      }
    }
    
    // Check floors below
    for (int i = currentFloor - 2; i >= 0; i--) {
      if (floorCalls[i]) {
        direction = DOWN;
        return i + 1;
      }
    }
  }
  
  return -1; // No calls pending
}

void moveToFloor(int targetFloor) {
  // Determine direction
  if (targetFloor > currentFloor) {
    direction = UP;
  } else if (targetFloor < currentFloor) {
    direction = DOWN;
  } else {
    return; // Already at target floor
  }
  
  // Move floor by floor
  while (currentFloor != targetFloor) {
    if (direction == UP) {
      currentFloor++;
    } else {
      currentFloor--;
    }
    
    // Update floor LED
    updateFloorLED(currentFloor);
    
    // Print status
    printFloorArrival(currentFloor);
    
    // Check if this floor was called
    if (floorCalls[currentFloor - 1]) {
      // Arrived at called floor
      openDoors();
      floorCalls[currentFloor - 1] = false; // Clear the call
      closeDoors();
    } else {
      // Just passing through - use non-blocking delay
      unsigned long startTime = millis();
      while (millis() - startTime < FLOOR_PAUSE_TIME) {
        readButtons(); // Continuously read buttons during travel
        delay(10);     // Small delay to prevent overwhelming the system
      }
    }
  }
}

// ============================================
// DOOR OPERATION FUNCTIONS
// ============================================

void openDoors() {
  doorOpen = true;
  digitalWrite(LED_DOOR_OPEN, HIGH);
  
  Serial.println("\n=== DOORS OPENING ===");
  Serial.print("Current Floor: ");
  Serial.println(currentFloor);
  Serial.print("Pending Calls: ");
  printPendingCalls();
  Serial.println("=====================\n");
  
  // Non-blocking delay to allow button reading during door open
  unsigned long startTime = millis();
  while (millis() - startTime < DOOR_OPEN_TIME) {
    readButtons(); // Continuously read buttons while doors are open
    delay(10);     // Small delay to prevent overwhelming the system
  }
}

void closeDoors() {
  doorOpen = false;
  digitalWrite(LED_DOOR_OPEN, LOW);
  Serial.println("Doors closing...\n");
}

// ============================================
// LED CONTROL FUNCTIONS
// ============================================

void updateFloorLED(int floor) {
  // Turn off all floor LEDs
  turnOffAllFloorLEDs();
  
  // Turn on current floor LED
  digitalWrite(ledPins[floor - 1], HIGH);
}

void turnOffAllFloorLEDs() {
  for (int i = 0; i < NUM_FLOORS; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// ============================================
// SERIAL OUTPUT FUNCTIONS
// ============================================

void printFloorArrival(int floor) {
  Serial.print("Elevator arriving at Floor ");
  Serial.println(floor);
  Serial.print("Direction: ");
  
  switch(direction) {
    case UP:
      Serial.println("UP");
      break;
    case DOWN:
      Serial.println("DOWN");
      break;
    case IDLE:
      Serial.println("IDLE");
      break;
  }
}

void printPendingCalls() {
  Serial.print("[");
  for (int i = 0; i < NUM_FLOORS; i++) {
    Serial.print("Floor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(floorCalls[i] ? "YES" : "NO");
    if (i < NUM_FLOORS - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("]");
}
