# r: pyserial
import serial
import time
import Rhino.Geometry as rg

# Initialize outputs
raw_data = []
accel = [0.0, 0.0, 0.0]        # Acceleration [g] (X, Y, Z)
gyro = [0.0, 0.0, 0.0]         # Angular velocity [deg/s] (X, Y, Z)
distance = -1.0                # Ultrasonic distance [cm]
gravity_vec = rg.Vector3d(0, 0, 1) # Gravity vector (Rhino Vector3d)
msg = ""

# Sensitivity scale factors (MPU6050 default settings)
ACCEL_SCALE = 16384.0  # LSB per 1g in ±2g range
GYRO_SCALE = 131.0     # LSB per 1deg/s in ±250deg/s range

# Retain global variables
if 'ser' not in globals():
    ser = None

if run:
    try:
        # Connect to port
        if ser is None or not ser.is_open:
            ser = serial.Serial(port, int(baud), timeout=0.1)
            time.sleep(0.2) # Wait for connection to stabilize
            msg = "Connected to {}".format(port)
        else:
            msg = "Port active"

        # Read all accumulated data in the buffer and get the latest line
        lines = []
        while ser.in_waiting > 0:
            raw_line = ser.readline()
            try:
                decoded = raw_line.decode('utf-8').strip()
                if decoded:
                    lines.append(decoded)
            except:
                pass

        # Parse the latest data and convert to physical units
        if lines:
            latest_line = lines[-1]
            parts = latest_line.split(',')
            if len(parts) == 7: # IMU(6) + Ultrasonic(1) = 7 elements
                # Convert IMU data to integers and distance to float
                raw_data = [int(p) for p in parts[:6]]
                distance = float(parts[6])
                
                # --- Scale 16-bit raw data to physical units ---
                # 1. Acceleration [g]
                ax = raw_data[0] / ACCEL_SCALE
                ay = raw_data[1] / ACCEL_SCALE
                az = raw_data[2] / ACCEL_SCALE
                accel = [ax, ay, az]

                # 2. Angular velocity [deg/s]
                gx = raw_data[3] / GYRO_SCALE
                gy = raw_data[4] / GYRO_SCALE
                gz = raw_data[5] / GYRO_SCALE
                gyro = [gx, gy, gz]

                # 3. Gravity vector (Vector3d directly usable in Rhino 3D space)
                gravity_vec = rg.Vector3d(ax, ay, az)

                msg = "Receiving: " + latest_line
            else:
                msg = "Malformed line: " + latest_line
        else:
            msg = "Waiting for data..."

    except Exception as e:
        msg = "Error: {}".format(str(e))
        if ser and ser.is_open:
            ser.close()
            ser = None
else:
    if ser and ser.is_open:
        ser.close()
        ser = None
        msg = "Disconnected"