use <wntrmute.scad>


ursBarrelHeight= 15;
ursBarrelRadius = 8;
ursBarrelOffset = 26;
ursDepth = 20;
ursWidth = 45;
ursEdge = 2;
ursCenter1Y = ursEdge + ursBarrelRadius + 1;
ursCenter2Y = ursCenter1Y + ursBarrelOffset;

nedges=100;
mspacing=15; // mount spacing
moffset=mspacing/2;
width = ursWidth+(2 * ursEdge);
echo("width=", width);
mdepth = 15;
mount_center = width / 2;

union() {
    translate([0, 0, PlateHeight()])
    difference() {
        rotate([0, -90, 0]) cube([22, width, PlateHeight()]);
        rotate([0, -90, 0]) translate([11, ursCenter1Y, -1])
            cylinder(h=ursBarrelHeight, r=ursBarrelRadius+0.1, $fn=nedges);
        rotate([0, -90, 0]) translate([11, ursCenter2Y, -1]) 
            cylinder(h=ursBarrelHeight, r=ursBarrelRadius+0.1, $fn=nedges);
    }
    
    difference() {
        
        translate([-PlateHeight(), 0, 0]) cube([mdepth, width, PlateHeight()]);
        translate([-PlateHeight(), 0, 0]) screw_hole(M3(), 10, mount_center - moffset, 10);
        translate([-PlateHeight(), 0, 0]) screw_hole(M3(), 10, mount_center + moffset, 10);
    }    
}
