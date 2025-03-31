#define TAP_PIN1 4   // GPIO4
int threshold = 30;          // Adjust based on calibration

// Timing thresholds in milliseconds
const unsigned long singleTapMaxDuration = 500;   // Max duration for a single tap
const unsigned long doubleTapMaxInterval = 500;   // Max interval between two taps for a double tap
const unsigned long tripleTapMaxInterval = 500;   // Max interval between three taps for a triple tap

enum TapType {
  NO_TAP,
  SINGLE_TAP,
  DOUBLE_TAP,
  TRIPLE_TAP
};

unsigned long lastTapTime = 0;
int tapCount = 0;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
}

void loop() {
  static bool touchRegistered = false;
  unsigned long currentTime = millis();
  uint16_t capacitiveValueLevel1 = touchRead(TAP_PIN1);

  if (capacitiveValueLevel1 < threshold) {
    if (!touchRegistered) {
      touchRegistered = true;
      tapCount++;
      lastTapTime = currentTime;
    }
  } else {
    touchRegistered = false;
  }

  if (tapCount > 0 && (currentTime - lastTapTime > doubleTapMaxInterval)) {
    TapType tapType = identifyTapType(tapCount);
    executeTapAction(tapType);
    tapCount = 0;
  }

  delay(10); // Short delay to debounce touch readings
}

TapType identifyTapType(int count) {
  switch (count) {
    case 1:
      return SINGLE_TAP;
    case 2:
      return DOUBLE_TAP;
    case 3:
      return TRIPLE_TAP;
    default:
      return NO_TAP;
  }
}

void executeTapAction(TapType tapType) {
  switch (tapType) {
    case SINGLE_TAP:
      Serial.println("Single Tap Detected");
      // Add action for single tap here
      break;
    case DOUBLE_TAP:
      Serial.println("Double Tap Detected");
      // Add action for double tap here
      break;
    case TRIPLE_TAP:
      Serial.println("Triple Tap Detected");
      // Add action for triple tap here
      break;
    default:
      break;
  }
}
