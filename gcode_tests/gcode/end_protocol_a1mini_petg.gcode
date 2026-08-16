; --- OPTIMIZED END PROTOCOL ---
; 1. Retract & Safe Z-Hop
G92 E0 ; Reset extrusion distance counter
G1 E-2 F1800 ; Retract 2mm filament to prevent oozing/stringing
G91 ; Switch to Relative positioning mode
G1 Z5 F3000 ; Lift Z axis by 5mm relative to current position
G90 ; Switch back to Absolute positioning mode
; 2. Park Carriage & Present Print
G28 X0 ; Home X axis
G1 Y170 F3000 ; Move Y axis forward to present the printed part (adjust Y value as needed)
; 3. Cooldown & Power Down
M106 S0 ; Turn off part cooling fan
M104 S0 ; Turn off nozzle heater
M140 S0 ; Turn off bed heater
M84 ; Disable stepper motors (X, Y, Z, E)
; --- END OF END PROTOCOL ---