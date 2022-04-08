use <wntrmute.scad>;

difference () {
    cube([100, 40, PlateHeight()]);
    screw_hole(M25(), 43, 2);
    screw_hole(M25(), 57, 2);

    rotate([0, 0, 15])
    ultrasonic_mount(33, 15);
    ultrasonic_mount(50, 24);
    rotate([0, 0, -15])
    ultrasonic_mount(63, 39);
}