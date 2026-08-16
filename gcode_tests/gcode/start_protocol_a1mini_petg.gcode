; --- OPTIMIZED START PROTOCOL ---
G90 ; Set positioning mode to Absolute
M82 ; Set extruder to Absolute mode
M106 S128 ; Turn part cooling fan to 50%
; 1. Preheating (Prevent oozing during homing)
M140 S75 ; Set bed temperature to 75C (non-blocking)
M104 S170 ; Set nozzle standby temperature to 170C (non-blocking)
M190 S75 ; Wait for bed temperature to reach 75C
M109 S170 ; Wait for nozzle standby temperature
; 2. Homing & Positioning
G28 ; Home all axes (X, Y, Z)
M420 S1 ; Enable Auto Bed Leveling mesh if available
G1 Z5 F3000 ; Lift nozzle 5mm to prevent bed collision
G1 X5 Y10 F5000 ; Move quickly to prime line start position (X=5, Y=10)
; 3. Final Nozzle Heating
M104 S240 ; Set target nozzle temperature to 240C
M109 S240 ; Wait for nozzle temperature to reach 240C
; 4. Purge Line (Prime & Clean for A1 mini purge tab)
G1 Z5 F3000
G1 X65 Y-10 F3000 ; Rapid move to purge tab left side
G1 Z0.1 F200     ; Lower nozzle to purge position
G92 E0            ; Reset extruder count
G1 X115 E15 F200 ; Purge while moving to X=115 (extrude 15mm)
G1 X125 F3000    ; Rapid move to the right to break the filament tail (wipe)
G1 Z1.0 F1000     ; Lift 1mm
; 5. Transition to Print
G92 E0 ; Reset extrusion distance counter for main print
; --- END OF START PROTOCOL ---