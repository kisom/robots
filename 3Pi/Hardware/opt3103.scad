use <wntrmute.scad>;

difference () {
    cube([30, 40, PlateHeight()]);
    screw_hole(M25(), 7, 5);
    screw_hole(M25(), 21, 5);
    screw_hole(M25(), 3, 35);
    screw_hole(M25(), 27, 35);
}