// mounting stand for robot

mountLength = 65;
mountWidth = 50;
baseHeight = 25;
bladeEnd = 11;

module blade(x) {
  translate([x, 0, baseHeight-1])
  cube([12, mountLength, bladeEnd]);  
};

difference() {
    union() {
        cube([50, mountLength, baseHeight]);
        blade(5);
        blade(33);   
    }
    translate([0, bladeEnd, baseHeight]) cube([mountWidth, mountLength-(2*bladeEnd), baseHeight]);
}