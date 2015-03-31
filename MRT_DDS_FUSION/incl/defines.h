#ifndef _DEFINES_H_
#define _DEFINES_H_

enum CoordinatesTypePosition { POLAR_2D, POLAR_3D, ABSOLUTE_2D, ABSOLUTE_3D, CARTESIAN_2D, CARTESIAN_3D };

const float earth_radius = 6371000;

#define DEGRAD		0.017453293   /* Costante di conversione da gradi in radianti */
#define RADDEG		57.295779513    /* Costante di conversione da radianti in gradi */

#endif