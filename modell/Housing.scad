width=160;
height=102;
thick=2;
border=8;
spacer=4;
spacerWidth=2;
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
