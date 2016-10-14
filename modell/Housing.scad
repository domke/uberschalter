width=160;
height=105;
thick=2;
border=8;
spacer=4;
spacerWidth=2;

difference() {
    union(){
        cube([width,height,thick], center=true);
        // Left side
        translate([(-width/2), (height/2), (-thick/2)]) {
            cube([width, 2, border]);
        }
        translate([(-width/2), (height/2)-spacerWidth, (-thick/2)]) {
            cube([width, spacerWidth, spacer]);
        }
        translate([(-width/2), (height/2)-spacerWidth, border-(thick/2)]) {
            cube([width, spacerWidth*2, spacer]);
        }

        // Right side
        translate([(-width/2), (-height/2), (-thick/2)]) {
            cube([width, 2, border]);
        }
        translate([(-width/2), (-height/2)+spacerWidth, (-thick/2)]) {
            cube([width, spacerWidth, spacer]);
        }
        translate([(-width/2), -((height/2)-(spacerWidth)+2), border-(thick/2)]) {
            cube([width, spacerWidth*2, spacer]);
        }
    }

    union()
    {
        step = 24;
        size=22;
        startpointX=-((width/2)-step)-5;
        startpointY=-((height/2)-step)-6;
        for (row=[3:0]) {
            for (col=[5:0]) {
                translate([startpointX+col*step, startpointY+row*step, 0]) {            
                    cube([size, size, thick*2], center=true);
                }
            }
        }
    }
}