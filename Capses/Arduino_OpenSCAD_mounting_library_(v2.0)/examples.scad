include <arduino.scad>

//Arduino boards
//You can create a boxed out version of a variety of boards by calling the arduino() module
//The default board for all functions is the Uno

dueDimensions = boardDimensions( DUE );
unoDimensions = boardDimensions( UNO );

//Board mockups
// Arduino UNO
*arduino();

// Arduino més gran
*translate( [unoDimensions[0] + 50, 0, 0] )
	arduino(DUE);

// Arduino de conector USB petit
*translate( [-(unoDimensions[0] + 50), 0, 0] )
	arduino(LEONARDO);

// Capses
translate([0, 0, -75]) {
    // Base Capsa amb Pareds
	*enclosure();

    // Rebora de la placa
	*translate( [unoDimensions[0] + 50, 0, 0] )
		//bumper(DUE);  // DUE
        bumper();       // UNO

	translate( [-(unoDimensions[0] + 50), 0, 0] ) union() {
		standoffs(LEONARDO, mountType=PIN);
		boardShape(LEONARDO, offset = 3);
	}
}

// Tapa
*translate([0, 0, 75]) {
	enclosureLid();
}
