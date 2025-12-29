# Fruit Weighing & Pricing System

An embedded systems project that implements an automated fruit weighing and pricing terminal using an ATmega328P, HX711 load cell amplifier, 16×2 LCD, and matrix keypad. The system supports separate Admin and User roles, real-time weight measurement, automatic price calculation, and persistent storage in EEPROM.

## 📌 Features

- **Dual-Role Operation**

  - Admin mode for managing fruit prices, password, income, and calibration.
  - User mode for normal shopping and checkout.

- **Real-Time Weighing**

  - HX711 24‑bit ADC with calibrated load cell.
  - Live weight display with small realistic variation during measurement.

- **Automatic Pricing**

  - Price per kilogram configurable for each fruit.
  - Item total and cart total computed automatically.

- **Data Persistence**

  - Prices, admin password, calibration data, and total income stored in EEPROM.
  - Survives power loss and restarts.

- **Secure Authentication**
  - Admin login with numeric password.
  - 3 attempts with lockout and error messages.

## 🧱 System Overview

The system is designed as a complete embedded POS terminal for a small fruit shop:

- **Admin can:**

  - Set or update prices for up to 5 fruits (e.g. Apple, Orange, Mango, Strawberry, Banana).
  - Change the admin password.
  - View and optionally reset total income.
  - Run a two-step calibration of the digital scale.

- **User can:**
  - Choose fruit from a list using keypad navigation.
  - Place fruit on the scale and see live weight.
  - Confirm item weight and add it to the cart.
  - Checkout and confirm payment to update total income.

## 🔌 Hardware Components

- **Microcontroller:** ATmega328P (Arduino-compatible, 16 MHz)
- **Load Cell + ADC:** HX711 24‑bit load cell amplifier
- **Display:** 16×2 LCD with I2C backpack
- **Input:** 4×4 matrix keypad (0–9, A–D, `*`, `#`)
- **Storage:** Internal EEPROM (for prices, password, calibration, income)
- **Programmer:** USBASP or compatible AVR ISP

Typical connections:

- HX711:
  - DOUT → PB4
  - SCK → PC5
- LCD:
  - I2C (SDA/SCL) on ATmega328P (A4/A5)
- Keypad:
  - 4 rows + 4 columns to GPIO pins

## 🧬 Software Architecture

The firmware follows a layered, modular structure:

- **Application Layer (`main.c`)**

  - Implements a finite state machine (FSM) with states such as:
    - `STATEROLESELECT`
    - `STATEADMINLOGIN`
    - `STATEADMINMENU`
    - `STATEUPDATEPRICE`
    - `STATEUPDATEPASSWORD`
    - `STATEVIEWINCOME`
    - `STATECALIBRATESCALE`
    - `STATEUSERBROWSEITEMS`
    - `STATEUSERWEIGHITEM`
    - `STATEUSERCHECKOUT`
    - `STATELOGOUT`
  - Coordinates user flow, menus, and error handling.

- **Application Data Layer (`app_data.c/.h`)**

  - Manages:
    - Fruit prices
    - Item names
    - Admin password
    - Total income
    - HX711 calibration parameters
  - Provides clean APIs to read/write data from EEPROM.

- **Driver Layer**

  - `hx711.c/.h` – HX711 driver, reading raw ADC, calibrated weight, calibration, optional simulation pattern.
  - `lcd.c/.h` – 16×2 I2C LCD driver (print strings, numbers, cursor control).
  - `keypad.c/.h` – Matrix keypad scanning with debouncing.
  - `eeprom.c/.h` – Typed EEPROM access (byte, float, double, integer, strings).

- **Support Files**
  - `micro_config.h` – MCU frequency, includes, and low-level setup.
  - `std_types.h` – Standard typedefs (uint8, sint32, float32, etc.).
  - `common_macros.h` – Bit manipulation macros and helpers.

## 🧮 State Machine Flow

High-level FSM:

```

STATEROLESELECT
├─(1)→ STATEADMINLOGIN → STATEADMINMENU
│        ├─A/1→ STATEUPDATEPRICE
│        ├─B/2→ STATEUPDATEPASSWORD
│        ├─C/3→ STATEVIEWINCOME
│        └─D/4→ STATECALIBRATESCALE → STATEADMINMENU
│
└─(2)→ STATEUSERBROWSEITEMS
└→ STATEUSERWEIGHITEM
└→ STATEUSERCHECKOUT
└→ STATEROLESELECT

```

- All transitions are driven by keypad input and completion of operations.
- Global variables track:
  - Current state
  - Current role (Admin/User)
  - Currently selected item
  - Session total
  - Authentication status

## ⚖️ Calibration and Measurement

### Calibration Flow (Admin)

1. **Tare (Offset):**

   - Remove all weight from the scale.
   - Admin selects “Calibrate Scale”.
   - System records current raw value as offset.

2. **Scale Factor:**
   - Place a 1.000 kg calibration weight on the scale.
   - System measures raw value and computes:
     - `scale = (raw_value - offset) / 1.0 kg`.
   - Calibration (offset + scale) is saved to EEPROM.

### Measurement Formula

The driver uses:

```

weight_kg = (raw_adc - offset) / scale;

```

- Negative values are clamped to 0.0.
- EEPROM-stored calibration is loaded at startup if present.

### Optional Simulation

For testing (e.g. Proteus), the HX711 driver supports a synthetic pattern during weighing:

- First ≈2 seconds: around **1.0 kg** with small ± noise (on the order of grams).
- Next ≈1 second: **2.0 kg** spike.
- Then returns to ~**1.0 kg**.

This is controlled by functions such as:

```

hx711_startSimulationPattern();
hx711_stopSimulationPattern();

```

and a `getWeight()` wrapper in `main.c`.

## ▶️ How to Build and Run

### 1. Clone the Repository

```

git clone https://github.com/ahmed522/fruit-scale-and-pricing-system.git
cd fruit-scale-and-pricing-system

```

### 2. Open in Your IDE

- Use Eclipse CDT (or another AVR-capable IDE).
- Import the project as an existing C/AVR project.

### 3. Configure Toolchain

- MCU: `ATmega328P`
- Clock: `F_CPU = 16000000UL` (or as set in `micro_config.h`)
- Compiler flags (example):

```

-Wall -Os -mmcu=atmega328p -DF_CPU=16000000UL

```

### 4. Build

- Build the project to produce a `.hex` file (typically in `Debug/` or `Release/`).

### 5. Flash to MCU

Using `avrdude` and USBASP (example):

```

avrdude -c usbasp -p m328p -U flash:w:Debug/fruit-scale.hex:i

```

Adjust file name and paths according to your build output.

### 6. Proteus Simulation (If Provided)

- Open the Proteus project file.
- Load the compiled HEX into the ATmega328P component.
- Run the simulation and interact using the virtual keypad and LCD.

## 🕹️ User Interaction

### Role Selection

- On startup, the system shows a role selection screen:
  - Press `1` → Admin mode
  - Press `2` → User mode

### Admin Mode

From the admin menu you can:

- **Update Prices**

  - Enter item index (1–5).
  - See current price.
  - Enter new price with decimal support.
  - Input is validated for range and format.

- **Change Password**

  - Verify current password.
  - Enter new password and confirm.
  - Numeric-only, with validation.

- **View Income**

  - Shows total income stored in EEPROM.
  - Optionally reset after confirmation.

- **Calibrate Scale**
  - Runs the 2-step calibration (tare + known weight).

### User Mode

- **Browse Items**

  - Use `A` / `B` to navigate fruit list.
  - Press `#` to select the highlighted item.

- **Weigh Item**

  - Place the fruit on scale when prompted.
  - Live weight is shown on LCD.
  - Press `#` to confirm current weight.

- **Add / Checkout**

  - After confirming weight, item price is computed.
  - Choose to:
    - Press `1` to add another item.
    - Press `0` to proceed to checkout.

- **Checkout**
  - Total session amount is displayed.
  - Confirm payment to add to total income.
  - System thanks the user and returns to role select.

## 📂 Files Overview

Some key files in this project:

- `main.c` – main loop, FSM, user/admin flows, display logic.
- `app_data.c/.h` – interface to EEPROM for prices, passwords, income, calibration.
- `hx711.c/.h` – HX711 load cell driver and measurement functions.
- `lcd.c/.h` – LCD driver via I2C.
- `keypad.c/.h` – keypad scan and key decoding.
- `eeprom.c/.h` – read/write helpers for different data types.
- `std_types.h`, `common_macros.h`, `micro_config.h` – shared types, macros, configuration.

## 🔧 Development and Testing

- Code is written in C for AVR-GCC.
- Structured for clarity and reusability, with documented header files.
- Tested in:
  - Simulated environment (Proteus).
  - Real hardware with ATmega328P + HX711 + load cell + LCD + keypad.

## 📝 Future Improvements

Possible ideas for extending the project:

- Add UART or I2C logging of transactions.
- Integrate a small thermal receipt printer.
- Support more than 5 items using paginated menus.
- Add RTC to timestamp transactions.
- Add UART/USB interface for PC configuration tool.

---

If you use or modify this project, feel free to open issues or share improvements via pull requests.

