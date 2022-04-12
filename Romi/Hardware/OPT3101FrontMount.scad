use <wntrmute.scad>;

// 23 29

difference() {
    cube([41, 41, 3]);
    screw_hole(M3(), 9, 5);
    screw_hole(M3(), 32, 5);
    screw_hole(M3(), 5, 12);
    screw_hole(M3(), 34, 12);
    screw_hole(M2(), 9, 36);
    screw_hole(M2(), 32, 36);
}