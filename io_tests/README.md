# I/O Tests

## Real-Time Interactive Mesh Deformation via External MCU

A real-time 3D geometry deformation system in Rhino 8 and Grasshopper. It receives sensor telemetry (MPU-6050 IMU & HC-SR04 Ultrasonic) from an external microcontroller via USB Serial (`pyserial`), driving parametric mesh deformations and generating smooth SubD surfaces on the fly.


https://github.com/user-attachments/assets/f19488fa-f765-464e-802b-4acdf98a3f7c


---

## 1. Requirements & Dependencies

* **Software**: Rhino 8 (Grasshopper / Python 3 Scripting), PlatformIO (C++)
* **Python Libraries**: `pyserial`
  * Automatically installed in Rhino 8 via the script header directive (`# r: pyserial`). No manual `pip install` required.

---

## 2. Hardware & Wiring

### Components
* Microcontroller: **Seeed Studio XIAO RP2040**
* IMU: **MPU-6050** (6-axis)
* Ultrasonic Sensor: **HC-SR04**
* Resistors: **1 kΩ, 2 kΩ** (1x each)

### Pinout
| Module | Pin | XIAO Pin | Note |
|---|---|---|---|
| **HC-SR04** | VCC | 5V | 5V Power |
| | Trig | D2 | GPIO |
| | Echo | **D3** | Via voltage divider (5V to 3.3V) |
| | GND | GND | |
| **MPU-6050** | VCC | 3V3 | |
| | SDA | D4 | I2C Data |
| | SCL | D5 | I2C Clock |
| | GND | GND | |

* **Voltage Divider**: `Echo -> [1kΩ] -> D3 <- [2kΩ] GND` (protects XIAO's 3.3V logic).

---

## 3. Architecture & Data Flow

```
[ MPU-6050 / HC-SR04 ]
          │ (I2C / GPIO)
          ▼
[ XIAO RP2040 (PlatformIO) ]
          │ (USB Serial @ 115200 bps)
          ▼
[ Rhino 8 / Grasshopper ]
          ├─ imu_sonic_serial.py  : Receive serial data (pyserial)
          ├─ Transform Components : Flow deformation via gravity vector
          ├─ mesh_wavy_deform.py  : 2-axis wave deformation via distance
          └─ SubD from Mesh       : Final output
```

---

## 4. Firmware

- **Framework**: PlatformIO (C++)
- **Functions**:
    - Reads MPU-6050 data and applies a high-pass filter (HPF) to remove gravity.
    - Measures distance (cm) via HC-SR04.
- **Serial Output**: `115200 bps` / CSV format (`lin_ax,lin_ay,lin_az,gx,gy,gz,dist\n`)

---

## 5. Grasshopper Pipeline

| **Stage** | **Script / Component** | **Description** |
| --- | --- | --- |
| **Data Receive** | `imu_sonic_serial.py` | Parses incoming serial data. Uses a `Trigger` component (100ms) to automatically update the GH canvas. |
| **Base Deform** | `Revolve` / `QuadRemesh` | Generates a mesh and applies Flow deformation based on IMU `gravity_vec`. |
| **Wave Generation** | `mesh_wavy_deform.py` | Applies 2-axis Sine waves along vertex normals. Adjusts wave amplitude and wavelength dynamically based on sensor `distance`. |
| **Final Output** | `SubD from Mesh` | Converts the deformed mesh to a smooth SubD surface. |

## 6. Setup Notes

- **Serial Port**: Update the port path (e.g., `/dev/cu.usbmodem14101` or `COM3`) in `imu_sonic_serial.py` to match your local machine.
- **Baud Rate**: Ensure both firmware and Grasshopper scripts are set to `115200 bps`.
