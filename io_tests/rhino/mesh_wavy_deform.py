import Rhino.Geometry as rg
import math

def ocean_wave(mesh, w1, w2, h, vec):
    # --- 1. Normalize the propagation direction vector for Wave 1 ---
    d_vec1 = rg.Vector3d(vec[0], vec[1], vec[2])
    d_vec1.Unitize()

    # Reference axis (Z-axis, or X-axis if parallel to Z-axis)
    o_axis = rg.Vector3d(0, 0, 1)
    if abs(d_vec1.Z) > 0.99:
        o_axis = rg.Vector3d(1, 0, 0)

    # Orthogonal axis of Wave 1 (for position calculation)
    o_vec1 = rg.Vector3d.CrossProduct(d_vec1, o_axis)
    o_vec1.Unitize()

    # --- 2. Calculate Wave 2 vector (90-degree orthogonal) ---
    o_vec2 = rg.Vector3d.CrossProduct(d_vec1, o_vec1)
    o_vec2.Unitize()

    # --- 3. Retrieve mesh vertices and normals ---
    pts = mesh.Vertices.ToPoint3dArray()
    mesh.Normals.ComputeNormals()
    normals = mesh.Normals

    new_pts = []
    for i, p in enumerate(pts):
        # Distances along the axes of Wave 1 and Wave 2
        dist1 = rg.Vector3d(p) * o_vec1
        dist2 = rg.Vector3d(p) * o_vec2

        # Calculate sine waves in two directions
        s1 = math.sin((dist1 / w1) * 2.0 * math.pi)
        s2 = math.sin((dist2 / w2) * 2.0 * math.pi)

        # Interference like ocean surface by multiplication (adjusted to stay within range of -1.0 to 1.0)
        val = h * (s1 * s2)

        # Deform along the normal direction
        n = rg.Vector3d(normals[i])
        new_p = p + n * val
        new_pts.append(new_p)

    # --- 4. Reconstruct the mesh ---
    res_mesh = rg.Mesh()
    for p in new_pts:
        res_mesh.Vertices.Add(p)

    for f in mesh.Faces:
        if f.IsQuad:
            res_mesh.Faces.AddFace(f.A, f.B, f.C, f.D)
        else:
            res_mesh.Faces.AddFace(f.A, f.B, f.C)

    return res_mesh

# Execution (Inputs: mesh, w1, w2, h, vec)
new_mesh = ocean_wave(mesh, w1, w2, h, vec)