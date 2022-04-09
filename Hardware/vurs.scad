use <wntrmute.scad>

ursHeight = 15;
ursRadius = 8;
nedges=100;

union() {
    difference() {
        rotate([90, 0, 0]) cube([22, 52, 2.5]);
        rotate([90, 0, 0]) translate([11, 42, -1]) 
            cylinder(h=ursHeight, r=ursRadius+0.1, $fn=nedges);
        rotate([90, 0, 0]) translate([11, 16, -1])
            cylinder(h=ursHeight, r=ursRadius+0.1, $fn=nedges);
    }
    
    difference() {
        cube([22, 10, 2.5]);
        screw_hole(M25(), 3, 7);
        screw_hole(M25(), 19, 7);
        screw_hole(M25(), 3, 3);
        screw_hole(M25(), 19, 3);
    }    
}
