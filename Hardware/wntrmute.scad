function M2() = 1;
function M25() = 1.25;
function M3() = 1.75;
function M4() = 2.1;
function PlateHeight() = 2.5;
    

module screw_hole(size, x, y, height=PlateHeight()+1) {
    union() {
       translate([x, y, -1])
       cylinder(h=height, r=size, $fn=60); 
    };
}

// x and y are the center point for the battery mount.
module battery_mount(x, y, size=M3(), rotation=0) {
    offset = 19.5 / 2;
    union() {
        screw_hole(size, x-offset, y);
        screw_hole(size, x+offset, y);
    }
}

module arduino_uno(x, y, size=M3(), rotation=0) {
    union() {
        screw_hole(size, x, y); 
        screw_hole(size, x+50.8, y-15.2);
        screw_hole(size, x+50.8, y-43.1);
        screw_hole(size, x, y-48.3);
    }
}

module robot_power_board(x, y, size=M3(), rotation=0) {
    union() {
        screw_hole(size, x, y);
        screw_hole(size, x+31.5, y);
        screw_hole(size, x, y+18.5);
        screw_hole(size, x+31.5, y+18.5);
    }
}

module raspberry_pi_zero(x, y, size=M3(), rotation=0) {
    union () {
        screw_hole(size, x, y);
        screw_hole(size, x+58, y);
        screw_hole(size, x, y+23);
        screw_hole(size, x+58, y+23);
    }
}

module raspberry_pi_3(x, y, size=M3(), rotation=0) {
    union () {
        screw_hole(size, x, y);
        screw_hole(size, x+58, y);
        screw_hole(size, x, y+49);
        screw_hole(size, x+58, y+49);
    }
}

module ultrasonic_mount(x, y, size=M3(), rotation=undef) {
    //rotate(rotation?rotation:[0,0,0])
    union() {
        screw_hole(size, x, y);
        screw_hole(size, x+13, y);
    }
}
