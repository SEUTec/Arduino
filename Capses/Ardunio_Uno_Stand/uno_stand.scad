/*
 * University of Illinois/NCSA
 * Open Source License
 *
 *  Copyright (c) 2007-2008,The Board of Trustees of the University of
 *  Illinois.  All rights reserved.
 *
 *  Copyright (c) 2012 Sam King
 *
 *  Developed by:
 *
 *  Professor Sam King in the Department of Computer Science
 *  The University of Illinois at Urbana-Champaign
 *      http://www.cs.illinois.edu/homes/kingst/Research.html
 *
 *       Permission is hereby granted, free of charge, to any person
 *       obtaining a copy of this software and associated
 *       documentation files (the "Software"), to deal with the
 *       Software without restriction, including without limitation
 *       the rights to use, copy, modify, merge, publish, distribute,
 *       sublicense, and/or sell copies of the Software, and to permit
 *       persons to whom the Software is furnished to do so, subject
 *       to the following conditions:
 *
 *          Redistributions of source code must retain the above
 *          copyright notice, this list of conditions and the
 *          following disclaimers.
 *
 *          Redistributions in binary form must reproduce the above
 *          copyright notice, this list of conditions and the
 *          following disclaimers in the documentation and/or other
 *          materials provided with the distribution.
 *
 *          Neither the names of Sam King, the University of Illinois,
 *          nor the names of its contributors may be used to endorse
 *          or promote products derived from this Software without
 *          specific prior written permission.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT.  IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS WITH THE SOFTWARE.
*/
Eps=0.01;

wall=1.5;   // old 2
// Placa Arduino
a_x = 53.5;
a_y = 69.5;  // UNO
a_y = 102.5;  //MEGA, DUE
a_z = 2;
groove=.5;

// Clip
clip_width = 15;
clip_wall = 1.5;    // old 2
clip_edge = 2.5;

// Constor Exterior
out_x = a_x + wall*2 - groove*2;
echo(out_x=out_x);
out_y = a_y + wall - groove;
echo(out_y=out_y);
out_z = 8.5;

// Hueco Interior
in_x = a_x - groove*2;
in_y = a_y - groove;
in_z = out_z - wall;

difference() {
    // Cos Principal
	cube([out_x,out_y,out_z]);
    // Hueco Interior
	translate([wall,wall,wall])
     cube([in_x, in_y, in_z+Eps]);
    // Ranura Interior
	translate([wall-groove, wall-groove, out_z-wall-a_z])
     cube([a_x, a_y, a_z]);
    difference(){
	 translate([wall-groove, out_y-clip_width, 0])
	  cube([out_x-wall*2+groove*2, clip_width, out_z]);
     translate([wall-groove+1, out_y-clip_width-1, 0])
      cube([out_x-wall*2+groove*2-2, clip_width, out_z]);
    }
}

// Pintes Clip
linear_extrude(height = out_z, center = false, convexity = 10, twist = 0)
 translate([0, out_y, 0])
  polygon([[0,0],[clip_edge,0],[0,clip_edge]]);

linear_extrude(height = out_z, center = false, convexity = 10, twist = 0)
	translate([out_x, out_y, 0])
	polygon([[0,0],[-clip_edge,0],[0,clip_edge]]);

// Externsió inferior
translate([wall-groove+2, out_y-clip_width+2.5, 0])
 cube([out_x-wall*2+groove*2-4, clip_width, wall]);
