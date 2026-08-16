# Custom G-code Generation with Grasshopper

Custom G-code generator for non-planar 3D printing paths built with Grasshopper.


https://github.com/user-attachments/assets/d4d58d08-c566-409f-a366-8e5581c282cd


---

## 1. Overview

Generating non-planar toolpaths and calculating extrusion values directly from geometry:

```
[ Input Geometry ]
│
▼
[ Non-Planar Curve Generation ] ─> Create wavy curves on geometry
│
▼
[ Curve Discretization ]        ─> Divide curves into points & extract XYZ coordinates
│
▼
[ Extrusion Calculation ]       ─> Compute E-value: (Extrusion Area / Filament Area) * Distance
│
▼
[ Format & Concatenate ]        ─> Join Start Protocol + Custom G-code + End Protocol
```

## 2. Sample G-code Specs & Target Machine

- **Target Machine:** Bambu Lab A1 mini
- **Nozzle Diameter:** 0.8 mm
- **Filament:** 1.75 mm PETG
- **Toolpath Characteristics:**
  - Fine Z-axis wavy texture pattern
  - Adjusted seams

## References

- [Advanced 3D Printing with Grasshopper®: Clay and FDM](https://www.food4rhino.com/en/resource/advanced-3d-printing-grasshopper-clay-and-fdm)
- [TU Delft | Generating 3D printing files (G-code) with Grasshopper](https://interactivetextbooks.tudelft.nl/rhino-grasshopper/Grasshopper_Rhino_course/2_Knowledge_base/Digital_fabrication/3D_Printing/%21index.html)
