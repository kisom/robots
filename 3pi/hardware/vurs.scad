use <wntrmute.scad>
ursHeight = 15;
ursRadius = 8;

union() {
    difference() {
        rotate([90, 0, 0]) cube([22, 50, 2]);
        rotate([90, 0, 0]) translate([11, 40, -10]) 
            cylinder(h=ursHeight, r=ursRadius+0.1, $fn=60);
        rotate([90, 0, 0]) translate([11, 13, -10])
            cylinder(h=ursHeight, r=ursRadius+0.1, $fn=60);
    }
    
    difference() {
        cube([22, 10, 2]);
        screw_hole(M25(), 3, 7);
        screw_hole(M25(), 19, 7);
        screw_hole(M25(), 3, 3);
        screw_hole(M25(), 19, 3);
    }
}
