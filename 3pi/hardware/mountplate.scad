use <wntrmute.scad>;

difference () {
    cube([30, 40, PlateHeight()]);
    screw_hole(M25(), 7, 5);
    screw_hole(M25(), 21, 5);
    screw_hole(M25(), 3, 35);
    screw_hole(M25(), 25.5, 35);
//    ultrasonic_mount(3, 15);
}