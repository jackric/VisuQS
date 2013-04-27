#pragma once
/***** Arbitrary parameters *****/
const float cutFrac = 0.13; // Carefully chosen to cut red from the BOTTOM of t)
const float overlap = 0.07; //  Size of objects is increased by this factor to incite overlapping
const float cr = 1.0; // Central Range for Fly-through
const float nearClipPlane = 0.001;
const float farClipPlane = 3100.0; // Must be beyond the depth of the text
// Lighting variables
const float ambient_light[] = {0.45, 0.55, 0.60};
const float source_light[] = {0.9, 0.8, 0.8};
const float light_pos[] = {400.0, 500.0, 300.0, 1.0};
const float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
const float mat_shininess[] = { 25.0 };
